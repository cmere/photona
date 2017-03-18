#ifndef SOCKETLIB_BLOCKBUFFER_H
#define SOCKETLIB_BLOCKBUFFER_H

#include <memory>

namespace SocketLib
{

/**
 *  Dynamic container using blocks of array as buffer. New block will be added as 
 *  needed, and existing un-used block will be deleted.
 */
class BlockBuffer
{
  public:
    BlockBuffer();
    ~BlockBuffer();

    static unsigned int getSizePerBlock() { return Block::BlockSize; }

    char* getDataPtr() const;
    char* getSpacePtr() const;

    // number of data in current block
    unsigned int getContinuousDataSize() const;

    // number of spaces in current block
    unsigned int getContinuousSpaceSize() const;

    unsigned int getTotalDataSize() const { return totalDataSize_; }
    
    // after read in data, adjust internal pointers and add blocks as needed.
    void resizePush(unsigned int numBytes);

    // after extract data, adjust internal pointers.
    void resizePop(unsigned int numBytes);

    void clearAll();

    unsigned int getdata(std::string& str, unsigned int count, unsigned int offset) const;
    unsigned int getdata(char* dest, unsigned int count, unsigned int offset) const;
    bool getline(std::string& dest, char delim, unsigned int offset) const;

    unsigned int append(const char*, unsigned int count, unsigned int offset);

  private:
    BlockBuffer(const BlockBuffer&) = delete;
    BlockBuffer& operator=(const BlockBuffer&) = delete;

  private:
    class Block {
      public:
        Block();
        ~Block();

        char* getDataPtr() const { return pData_; }
        char* getSpacePtr() const { return pSpace_; }

        unsigned int getDataSize() const { return pData_ ? pSpace_ - pData_ : 0; }
        unsigned int getSpaceSize() const { return pEnd_ - pSpace_; }

        bool isEmpty() const { return pData_ == nullptr && pSpace_ == pBegin_; }
        bool isFull() const { return pSpace_ == pEnd_; }

        unsigned int resizePush(unsigned int numBytes);
        unsigned int resizePop(unsigned int numBytes);

        unsigned int getdata(char* dest, unsigned int count, unsigned int offset) const;
        bool getline(std::string& dest, char delim, unsigned int offset) const;

      private:
        Block(const Block&) = delete;
        Block& operator==(const Block&) = delete;

      private:
        static const unsigned int BlockSize = 1024 * 1024;
        char bytes_[BlockSize];
        char* const pBegin_ = bytes_;
        char* const pEnd_ = bytes_ + BlockSize;
        char* pData_ = nullptr;
        char* pSpace_ = pBegin_;

        std::unique_ptr<Block> next_ = nullptr;
        friend class BlockBuffer;
    };

    std::unique_ptr<Block> currDataBlock_;
    Block* currSpaceBlock_;
    unsigned int totalDataSize_ = 0;
};

} // namespace SocketLib

#endif // SOCKETLIB_BLOCKBUFFER_H


