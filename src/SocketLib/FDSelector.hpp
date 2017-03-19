#ifndef SOCKETLIB_FDSELECTOR_H
#define SOCKETLIB_FDSELECTOR_H

#include "SocketID.hpp"
#include <map>
#include <memory>
#include <set>

namespace SocketLib
{

class ISelectable;

/**
 * encapsulate select() function.
 */
class FDSelector
{
  public:
    void addToReadSelectable(std::shared_ptr<ISelectable>);
    void addToWriteSelectable(std::shared_ptr<ISelectable>);
    //void addToExceptSelectable(shared_ptr<ISelectable> sock);
    void removeFromAll(std::shared_ptr<ISelectable>);

    int select(timeval* timeout = nullptr);

    std::set<std::shared_ptr<ISelectable>> getReadyToRead() const { return readyToReadSockets_; }
    std::set<std::shared_ptr<ISelectable>> getReadyToWrite() const { return readyToWriteSockets_; }
    //std::set<shared_ptr<ISelectable>> getReadyToExcept() const;

    unsigned int getNumberSelectables() const { return readSelectables_.size() + writeSelectables_.size(); }

  private:
    // 'select' on these file descriptors.
    std::map<SocketID, std::shared_ptr<ISelectable>> readSelectables_;
    std::map<SocketID, std::shared_ptr<ISelectable>> writeSelectables_;
    //std::map<int, shared_ptr<ISelectable>> exceptSelectableByFD_;

    // after 'select' return, remaining file descriptors.
    std::set<std::shared_ptr<ISelectable>> readyToReadSockets_;
    std::set<std::shared_ptr<ISelectable>> readyToWriteSockets_;
    //std::set<int> readyToExceptFDs_;
};

}

#endif // SOCKETLIB_FDSELECTOR_H

