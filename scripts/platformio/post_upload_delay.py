Import("env")

import time

def after_upload(source, target, env):
    print("Delay while uploading...")
    time.sleep(1)
    print("Done!")

env.AddPostAction("upload", after_upload)

# Replace upload with tycmd
def on_upload(source, target, env):
    print(source, target)
    firmware_path = str(source[0])
    # do something
    env.Execute("tycmd upload {}".format(firmware_path))

env.Replace(UPLOADCMD=on_upload)


