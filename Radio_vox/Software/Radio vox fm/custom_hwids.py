Import("env")

board_config = env.BoardConfig()
print(env.BoardConfig().get("build.hwids")[0][0])

board_config.update("vendor", "Polaxis")
print(env.BoardConfig().get("vendor"))
board_config.update("build.usb_product", "FM Radio")
print(env.BoardConfig().get("build.usb_product"))

# should be array of VID:PID pairs
board_config.update(
    "build.hwids", [["0x239A", "0x0107"]]
)  # VID PID 2341 arduino 0243 sinovox 239A adafruit
print(env.BoardConfig().get("build.hwids")[0][0])

# Emy 0100
# Kaiwa 0101
# Mea 0102
# Vax 0103
# Robovox 0104
# Sino 0105
# Speakjet 0106
# FM Radio 0107
