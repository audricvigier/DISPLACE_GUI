#ifndef IPCQUEUE_H
#define IPCQUEUE_H

#include <ipcmsgtypes.h>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <cstddef>

/** @brief implements a CircularBuffer to be used in a boost::interprocess::managed_heap_memory
 * @note DO NOT implements ANY virtual method in this class! Pointers are not portable across processes!
 */
class IpcQueue
{
public:
    IpcQueue();
    ~IpcQueue();

    bool push(IpcMessageTypes type, void *buffer, size_t len);
    IpcMessageTypes pickOrWait(void *buffer, size_t maxlen, size_t *len);

    static void forceCleanup();
private:
    bool empty() const;
    bool full() const;
    size_t space_available() const;
    bool push (char byte);
    char pop();

    struct MessageManager {
        boost::interprocess::interprocess_mutex mutex;
        boost::interprocess::interprocess_condition cond_notempty;
        boost::interprocess::interprocess_condition cond_notfull;

        static const size_t BufferSize = 512*1024;

        char buffer[BufferSize];
        int head;
        int tail;
        int size;

        MessageManager()
            : mutex(),
              cond_notempty(),
              cond_notfull(),
              head(0), tail(0),
              size(BufferSize)
        {
            
        }
    };

    static const size_t SharedMemorySize;
    static const char *SharedListName;

    boost::interprocess::shared_memory_object shmobj;
    boost::interprocess::mapped_region mapreg;

//    boost::interprocess::managed_shared_memory sharedMemory;
//    boost::interprocess::managed_heap_memory::handle_t sharedHandle;
    MessageManager *mManager;
};

#endif // IPCQUEUE_H