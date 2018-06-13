import os
import sys
import thread
import threading
import time
from lame import *
import ccolor


class lame_mt_util:
    
    __locker = threading.Lock()
    __color = ccolor.ConsoleColor()

    @classmethod
    def DisplayScanResult(cls , fname , result):

        cls.__locker.acquire()

        cls.__color.set_green_text()
        sys.stdout.write(fname)

        if result is not None:
            cls.__color.set_red_text()
            sys.stdout.write(" " + "  Infected: " + result.vname + " (" + result.engid+")")
        
        sys.stdout.write('\n')

        cls.__locker.release()

class safeq:

    def __init__(self , size = 100):
        self.__size = size
        self.__locker = threading.Lock()
        self.__queue = list()


    def enqueue(self , s , force = True):
        
        if s is None:
            return False

        ret = False

        if force:
            while True:

                self.__locker.acquire()

                if len(self.__queue) < self.__size:
                    self.__queue.append(s)
                    ret = True
 
                self.__locker.release()

                if ret:
                    return True

                time.sleep(0.1)

        else :
            
            self.__locker.acquire()

            if len(self.__queue) < self.__size:
                self.__queue.append(s)
                ret = True

            self.__locker.release()

            return ret


    def dequeue(self):

        vl = None

        self.__locker.acquire()

        if len(self.__queue) > 0:
            vl = self.__queue[0]
            self.__queue.remove(vl)

        self.__locker.release()

        return vl

    
    def Count(self):

        _count = 0

        self.__locker.acquire()
        _count = len(self.__queue)
        self.__locker.release()

        return _count


class travel_worker:

    def __init__(self , fq):
        self.__scan_file_queue = fq
        self.__travel_path_queue = safeq()
        self._thread = None
        self._event = threading.Event()

    def run(self):
        self._thread = threading.Thread(target = self.__run , args = ())
        self._thread.start()
        self._event.set()

    def append_scan_path(self , path):
        
        if path is None:
            return False

        self.__travel_path_queue.enqueue(path)
        
        return True

    def wait(self):
        if self._thread is None:
            return

        if not self._thread.isAlive():
            return
        
        self._event.wait()
    
    def __run(self):

        self._event.wait()
        self._event.clear()

        while True:

            _path = self.__travel_path_queue.dequeue()
            if _path is None:
                self.__scan_file_queue.enqueue("exit")
                break

            self.__travel(_path)

        self._event.set()


    def __travel(self , path):

        if path is None:
            return

        if os.path.isdir(path):

            files = os.listdir(path)
            for _file in files:
                _path = os.path.join(path , _file)

                if os.path.isfile(_path):
                    self.__scan_file_queue.enqueue(_path)
                elif os.path.isdir(_path):
                    self.__travel(_path)
                else:
                    pass

        elif os.path.isfile(path):

            self.__scan_file_queue.enqueue(path)

        else:
            pass



class scan_worker:

    def __init__(self ,lamep ,fq):
        self.__scan_file_queue = fq
        self.__lame = Lame(lamep)
        self.__running = False
        self.__thread = None
        self.file_count = 0
        self.virus_count = 0
        self.elapse_time = 0
        self.__event = threading.Event()

    
    def run(self , dbf):

        if self.__running:
            return True
        
        if dbf is None:
            return False

        if not self.__lame.Load(dbf):
            return False

        self.__running = True

        self.__thread = threading.Thread(target = self.__scan , args = ())
        self.__thread.start()
        self.__event.set()

        return True

    def wait(self):
        
        if self.__thread is None:
            return

        if not self.__thread.isAlive():
            return
        
        self.__event.wait()

    def __scan(self):

        self.__event.wait()
        self.__event.clear()

        _start_time = time.time()
        while True:

            _path = self.__scan_file_queue.dequeue()
            if _path is None:
                time.sleep(0.1)
                continue

            if _path == 'exit':
                self.__scan_file_queue.enqueue("exit")
                break

            self.file_count += 1

            result = self.__lame.ScanFile(_path)
            if result is not None:
                self.virus_count += 1

            lame_mt_util.DisplayScanResult(_path , result)

        self.elapse_time = time.time() - _start_time

        self.__lame.Unload()

        self.__event.set()



class scanner:

    def __init__(self , lpath):
        self._lpath = lpath
        self._scan_path_queue = safeq()
        self._vdb = None
        self._travel_worker = travel_worker(self._scan_path_queue)
        self._workers = list()
        

    def load(self):
        
        if self._lpath  is None:
            return False

        self._vdb = VirusDb(self._lpath)
        if not self._vdb.OpenVdb(None):
            return False

        return True

        
    
    def unload(self):
        if self._vdb is not None:
            return

        self._vdb.CloseVdb()

    def run(self , sc):

        if self._lpath is None:
            return

        if self._vdb is None:
            return

        self._travel_worker.run()
        
        idx = 0
        while idx < sc:
            _scan_work = scan_worker(self._lpath , self._scan_path_queue)
            _scan_work.run(self._vdb)
            self._workers.append(_scan_work)
            idx += 1


        self._travel_worker.wait()

        _scan_file_count = 0
        _virus_count = 0
        _elapse_time = 0

        for _scan_work in self._workers:
            _scan_work.wait()

            _scan_file_count += _scan_work.file_count
            _virus_count += _scan_work.virus_count
        
            if _elapse_time < _scan_work.elapse_time:
                _elapse_time = _scan_work.elapse_time

        print("")
        print("Files: " + str(_scan_file_count))
        print("Virus: " + str(_virus_count))
        print("elapse: " + str(_elapse_time) + "(s)")



    def append_scan_path(self , path):

        if path is None:
            return

        self._travel_worker.append_scan_path(path)




def main(argv):


    _lame_path = "E:\\work\\svn\\rame\\build\\release"
    _scanner = scanner(_lame_path)

    _worker_count = 4
    for param in argv:
        _param = str.lower(param)
        idx = _param.find("-workers=")
        if idx == 0:
            if len(_param) == 9:
                continue
            
            vl = _param[9:]
            _vl = int(vl)
            if _vl > 0 and _vl < 10:
                _worker_count = _vl

        else:
            _scanner.append_scan_path(param)

    if not _scanner.load():
        return

    _scanner.run(_worker_count)
    _scanner.unload()


def exit(signum, frame):  
    sys.exit()  

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit)  
    signal.signal(signal.SIGTERM, exit)
    main(sys.argv[1:])

    