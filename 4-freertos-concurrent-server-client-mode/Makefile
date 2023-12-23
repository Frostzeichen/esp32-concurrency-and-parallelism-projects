$(eval VSCODE_WINDOWNAME = $(shell xdotool getwindowfocus getwindowname))

setup:
	wmctrl -r "$(VSCODE_WINDOWNAME)" -e 0,10,48,1400,978
	alacritty -T "jellyfish - monitor" --hold &
	sleep .2
	wmctrl -r "jellyfish - monitor" -e 0,1414,48,492,978
	sleep .9
	wmctrl -a "$(VSCODE_WINDOWNAME)"

upload-and-monitor:
	wmctrl -a "jellyfish - monitor"
	xdotool key ctrl+c
	xdotool key enter
	sleep .2
	xdotool type "~/.platformio/penv/bin/platformio run --target upload && ~/.platformio/penv/bin/platformio run --target monitor"
	xdotool key enter
	wmctrl -a "$(VSCODE_WINDOWNAME)"

upload:
	wmctrl -a "jellyfish - upload"
	xdotool type "~/.platformio/penv/bin/platformio run --target upload"
	xdotool key enter
	wmctrl -a "$(VSCODE_WINDOWNAME)"

monitor:
	wmctrl -a "jellyfish - monitor"
	xdotool type "~/.platformio/penv/bin/platformio run --target monitor"
	xdotool key enter
	wmctrl -a "$(VSCODE_WINDOWNAME)"

build:
	platformio run

close:
	wmctrl -c "jellyfish - monitor"