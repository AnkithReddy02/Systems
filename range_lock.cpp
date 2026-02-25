#include <bits/stdc++.h>
using namespace std;

class RangeLockManager {
public:
  enum class RequestType { READ, WRITE };

  struct RangeLockRequest {
    RangeLockRequest(int p_start, int p_end, RequestType p_request_type) :
      start(p_start),
      end(p_end),
      request_type(p_request_type) {

    }

    int start;
    int end;   // [start, end]
    RequestType request_type;
  };

  void RequestRangeLock(int start, int end, RequestType request_type) {
    unique_lock<mutex> lock(mtx_);

    cv_.wait(lock, [this, start, end, request_type]() {
      return CanGrantAccess(start, end, request_type);
    });

    active_locks_.emplace_back(start, end, request_type);
  }

  void ReleaseRangeLock(int start, int end, RequestType request_type) {
    unique_lock<mutex> lock(mtx_);

    // TODO: Optimize with Binary Search.
    for (auto it = active_locks_.begin(); it != active_locks_.end(); ++it) {
      if (it->start == start &&
          it->end == end &&
          it->request_type == request_type) {
          active_locks_.erase(it);
          break;
      }
    }

    cv_.notify_all();
  }

private:
  bool CanGrantAccess(int start, int end, RequestType request_type) {
    for (const auto& active_request : active_locks_) {
      // Inclusive overlap check.
      bool overlaps =
        active_request.start <= end && start <= active_request.end;

      if (!overlaps) continue;

      // Conflict if either side is WRITE.
      if (active_request.request_type == RequestType::WRITE ||
          request_type == RequestType::WRITE) {
        return false;
      }
    }
    return true;
  }

private:
  vector<RangeLockRequest> active_locks_;

  condition_variable cv_;

  mutex mtx_;
};

int main() {
  return 0;
}