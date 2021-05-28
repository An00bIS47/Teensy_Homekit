Import("env")

import time

def after_upload(source, target, env):
    print("Delay while uploading...")
    time.sleep(1)
    print("Done!")

env.AddPostAction("upload", after_upload)
