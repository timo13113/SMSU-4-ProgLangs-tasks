import time
import platform


def get_max_memory_usage(log=None, ret=False):
    if platform.uname()[0] == 'Linux':
        import resource
        if ret:
            return resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        else:
            print("peak used phys memory:",
                  resource.getrusage(resource.RUSAGE_SELF).ru_maxrss, "KB", file=log)
    elif platform.uname()[0] == 'Windows':
        import os
        from win32api import OpenProcess, CloseHandle
        from win32process import GetProcessMemoryInfo

        pid = os.getpid()
        hprocess = OpenProcess(0x0400 | 0x0010, False, pid)
        mem_info = GetProcessMemoryInfo(hprocess)
        CloseHandle(hprocess)
        if ret:
            return mem_info['PeakWorkingSetSize'] / 1024
        else:
            print("currently used phys memory:", mem_info['WorkingSetSize'], "B", file=log)
            print("currently used phys memory:", mem_info['WorkingSetSize'] / 1024 ** 2, "MB", file=log)
            print("peak used phys memory:", mem_info['PeakWorkingSetSize'], "B", file=log)
            print("peak used phys memory:", mem_info['PeakWorkingSetSize'] / 1024 ** 2, "MB", file=log)


def timeit_and_log(name: str, log=None):
    def wrapper(func):
        def timed_function(*args, **kwargs):
            s = time.time_ns()
            res = func(*args, **kwargs)
            e = time.time_ns()
            print(name + " took", (e - s)/1_000_000, "ms to run", file=log)
            return res, (e - s)/1_000_000
        return timed_function
    return wrapper
