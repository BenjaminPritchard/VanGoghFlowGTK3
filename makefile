vangoghflow: vangoghflow.c vangoghflow.h screen.c screen.h tray.c tray.h keyboard.c keyboard.h  
	clang -g -O0 -std=c11 -Wdeprecated vangoghflow.c tray.c screen.c keyboard.c -o vangoghflow -lappindicator3 -I/usr/include/libdbusmenu-glib-0.4/libdbusmenu-glib/ -I/usr/include/libappindicator-0.1/libappindicator `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -lX11 -lXi
	
clean:
	rm -f vangoghflow
