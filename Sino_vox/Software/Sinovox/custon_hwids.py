
Import("env")

board_config = env.BoardConfig()
print(env.BoardConfig().get("build.hwids")[0][0])

board_config.update("vendor","Polaxis")
print(env.BoardConfig().get("vendor"))
board_config.update("build.usb_product","SinoVox")
print(env.BoardConfig().get("build.usb_product"))

# should be array of VID:PID pairs
board_config.update("build.hwids", [["0x2341", "0x0243"]]) #VID PID 2341 arduino 0243 sinovox 239A adafruit
print(env.BoardConfig().get("build.hwids")[0][0])

