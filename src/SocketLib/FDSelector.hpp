#ifndef SOCKETSERVER_FDSELECTOR_H
#define SOCKETSERVER_FDSELECTOR_H

#include <map>
#include <memory>
#include <set>

namespace SocketServer
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

    std::set<std::shared_ptr<ISelectable>> getReadyToRead() const;
    std::set<std::shared_ptr<ISelectable>> getReadyToWrite() const;
    //std::set<shared_ptr<ISelectable>> getReadyToExcept() const;

  private:
    // 'select' on these file descriptors.
    std::map<int, std::shared_ptr<ISelectable>> readSelectableByFD_;
    std::map<int, std::shared_ptr<ISelectable>> writeSelectableByFD_;
    //std::map<int, shared_ptr<ISelectable>> exceptSelectableByFD_;

    // after 'select' return, remaining file descriptors.
    std::set<int> readyToReadFDs_;
    std::set<int> readyToWriteFDs_;
    //std::set<int> readyToExceptFDs_;
};

}

#endif // SOCKETSERVER_FDSELECTOR_H

