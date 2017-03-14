#include "include/first.hpp"
#include "BlockBuffer.hpp"

#include <list>

using namespace std;

namespace SocketServer 
{

BlockBuffer::BlockBuffer()
{
  currDataBlock_.reset(new Block());
  currSpaceBlock_ = currDataBlock_.get();
}

char* 
BlockBuffer::getDataPtr() const 
{
  return currDataBlock_->getDataPtr();
}

char* 
BlockBuffer::getSpacePtr() const 
{
  return currSpaceBlock_->getSpacePtr();
}

unsigned int 
BlockBuffer::getContinuousDataSize() const 
{
  return currDataBlock_->getDataSize();
}

unsigned int 
BlockBuffer::getContinuousSpaceSize() const 
{ 
  return currSpaceBlock_->getSpaceSize();
}

void
BlockBuffer::clearAll()
{
  currDataBlock_.reset(new Block());
  currSpaceBlock_ = currDataBlock_.get();
  totalDataSize_ = 0;
}

void
BlockBuffer::resizePush(unsigned int numBytes)
{
  totalDataSize_ += numBytes;
  while ((numBytes -= currSpaceBlock_->resizePush(numBytes)) > 0) {
    logger << logger.debug << "resizePush: numBytes=" << numBytes << endlog;
    currSpaceBlock_ = currSpaceBlock_->next_.get();
  }
  if (currSpaceBlock_->isFull()) {
    unique_ptr<Block> newBlock(new Block());
    currSpaceBlock_->next_ = move(newBlock);
    currSpaceBlock_ = currSpaceBlock_->next_.get();
    logger << logger.debug << "new Block" << currSpaceBlock_ << endlog;
  }
}

unsigned int
BlockBuffer::Block::resizePush(unsigned int numBytes) 
{
  if (pData_ == nullptr) {
    pData_ = pBegin_;
  }
  unsigned int numSpaces = pEnd_ - pSpace_;
  if (numSpaces > numBytes) {
    pSpace_ += numBytes;
    return numBytes;
  }
  else {
    pSpace_ = pEnd_;
    return numSpaces;
  }
}

void
BlockBuffer::resizePop(unsigned int numBytes)
{
  totalDataSize_ -= numBytes;
  while ((numBytes -= currDataBlock_->resizePop(numBytes)) > 0) {
    if (currSpaceBlock_ == currDataBlock_.get()) {
      currSpaceBlock_ = currDataBlock_->next_.get();
    }
    currDataBlock_ = move(currDataBlock_->next_);
  }
  if (currDataBlock_->isEmpty() && currDataBlock_->next_) {
    if (currSpaceBlock_ == currDataBlock_.get()) {
      currSpaceBlock_ = currDataBlock_->next_.get();
    }
    currDataBlock_ = move(currDataBlock_->next_);
  }
}

unsigned int
BlockBuffer::Block::resizePop(unsigned int numBytes) 
{
  unsigned int numExistingData = pSpace_ - pData_;
  if (numExistingData > numBytes) {
    pData_ += numBytes;
    return numBytes;
  }
  else {
    pData_ = nullptr;
    pSpace_ = pBegin_;
    return numExistingData;
  }
}

unsigned int
BlockBuffer::getdata(string& str, unsigned int count, unsigned int offset) const
{
  if (count == 0) {
    return 0;
  }
  unique_ptr<char> buf(new char[count]);
  unsigned int numPopBytes = getdata(buf.get(), count, offset);
  str.assign(buf.get(), numPopBytes);
  return numPopBytes;
}

unsigned int
BlockBuffer::getdata(char* dest, unsigned int count, unsigned int offset) const
{
  unsigned int totalPopped = 0;
  Block* block = currDataBlock_.get();
  while (count > 0) {
    unsigned int numPopBytes = block->getdata(dest, count, offset);
    dest += numPopBytes;
    count -= numPopBytes;
    totalPopped += numPopBytes;
    offset = 0;
    if (block->next_) {
      block = block->next_.get();
    }
    else {
      break;
    }
  }

  if (count > 0) {
    logger << "BlockBuffer getdata: no more data, missing " << count << endlog;
  }
  return totalPopped;
}

unsigned int
BlockBuffer::Block::getdata(char* dest, unsigned int count, unsigned int offset) const
{
  unsigned int dataSize = getDataSize();
  if (dataSize > count + offset) {
    memcpy(dest, pData_ + offset, count);
    return count;
  }
  else {
    memcpy(dest, pData_ + offset, dataSize - offset);
    return dataSize - offset;
  }
}

bool
BlockBuffer::getline(std::string& dest, char delim, unsigned int offset) const
{
  Block* block = currDataBlock_.get();
  while (!block->getline(dest, delim, offset)) {
    offset = 0;
    if (block->next_) {
      block = block->next_.get();
    }
    else {
      return false;
    }
  }
  return true;
}

bool
BlockBuffer::Block::getline(std::string& dest, char delim, unsigned int offset) const
{
  if (isEmpty()) {
    return false;
  }

  char c = 0;
  char* p = pData_ + offset;
  while ((c = *p++) != delim) {
    dest.push_back(c);
    if (p == pSpace_) {
      return false;
    }
  }
  return true;
}

unsigned int
BlockBuffer::append(const char* data, unsigned int count, unsigned int offset)
{
  if (count == 0) {
    return 0;
  }

  unsigned int bytesAppended = count;

  // find block starting from offset
  Block* spaceBlock = currSpaceBlock_;
  while (spaceBlock->getSpaceSize() <= offset) {
    offset -= spaceBlock->getSpaceSize();
    spaceBlock = spaceBlock->next_.get();
    if (!spaceBlock) {
      // append() should be called sequentially: append(Field_1); append(Field_2); ...
      logger << "error: space block should not be null in appending. " << count << " " << offset << endlog;
      return 0;
    }
  }

  char* pSpace = spaceBlock->pSpace_ + offset;
  // start writing
  while (count > 0) {
    unsigned int availableSpaces = spaceBlock->pEnd_ - pSpace;
    if (availableSpaces > count) {
      memcpy(pSpace, data, count);
      count = 0;
      break;
    }
    else {
      memcpy(pSpace, data, availableSpaces);
      count -= availableSpaces;
      // create a new block
      unique_ptr<Block> newBlock(new Block());
      spaceBlock->next_ = move(newBlock);
      spaceBlock = spaceBlock->next_.get();
      logger << logger.debug << "new Block" << spaceBlock << endlog;
      pSpace = spaceBlock->pBegin_;
    }
  }

  return bytesAppended;
}

};  // namespace SocketServer
