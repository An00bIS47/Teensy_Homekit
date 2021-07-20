import serial
import collections
import signal
import re
from datetime import datetime

MAX_LOG_LINES = 500

class GracefulKiller:
  kill_now = False
  def __init__(self):
    signal.signal(signal.SIGINT, self.exit_gracefully)
    signal.signal(signal.SIGTERM, self.exit_gracefully)

  def exit_gracefully(self, *args):
    self.kill_now = True



def escape_ansi(line):
    ansi_escape =re.compile(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]')
    return ansi_escape.sub('', line)

if __name__ == '__main__':
  log = collections.deque(maxlen=MAX_LOG_LINES)
  ts_start = datetime.now()
  ts_end = ts_start
  
  killer = GracefulKiller()
  with serial.Serial('/dev/ttyACM0', 115200, timeout=1) as ser:
    while not killer.kill_now:
      data_raw = ser.readline()

      line = ser.readline().decode().strip()
      if line:
        log.append(data_raw)
        print(log[-1].decode("utf-8"), end="")
        ts_end = datetime.now()
      else:
        break

  print('\033[m')
  print("\n\n\n\n\n\n\nThe last 100 Lines:")
  for l in log:
    #print(escape_ansi(l.decode("utf-8")), end="")
    print(l.decode("utf-8"), end="")
  print('\033[m')

  print('\033[m')
  print("Summary:")
  print("Runtime from {} to {}".format(ts_start.strftime('%Y-%m-%d-%H:%M:%S'), ts_end.strftime('%Y-%m-%d-%H:%M:%S')))
  print("Duration: {}".format(ts_end - ts_start))
  if len(log) > 1:
    print("Last Line: \n{}".format(log[-1].decode("utf-8")), end="")
  print('\033[m')
  
  filename = "./reports/logging_" + datetime.today().strftime('%Y-%m-%d-%H:%M:%S') + ".log"
  with open(filename, 'w') as f:
    for l in log:
        f.write(escape_ansi(l.decode("utf-8")))

        
    f.write('\n\n\n\n\n\n')
    f.write("Summary:\n")
    f.write("Runtime from {} to {}\n".format(ts_start.strftime('%Y-%m-%d-%H:%M:%S'), ts_end.strftime('%Y-%m-%d-%H:%M:%S')))
    f.write("Duration: {}\n".format(ts_end - ts_start))
    if len(log) > 1:
        f.write("Last Line: \n{}\n".format(log[-1].decode("utf-8")))

