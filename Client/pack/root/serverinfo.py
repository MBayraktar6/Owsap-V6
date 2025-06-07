import app
import localeInfo

CLIENT_VERSION = "1"
app.ServerName = None
SERVER_1 = "|cffffd500|h Alena2"
SRV1 = {
	"name":"|cffffd500|h Alena2",
	"host":"192.168.1.50",
	"auth1":11150,
	"ch1":12101,
	"ch2":12101,
	"ch3":12101,
	"ch4":12101,
	"ch5":12101,
	"ch6":12101,
		}

STATE_NONE = '|cffff0000OFF'

STATE_DICT = {
	0 : "|cffff0000Off",
	1 : "|cff00ff00NORMAL",
	2 : "|cffffff00BUSY",
	3 : "|cffff6060FULL"
	}

SERVER1_CHANNEL_DICT = {
	1:{"key":11,"name":"CH1","ip":SRV1["host"],"tcp_port":SRV1["ch1"],"udp_port":SRV1["ch1"],"state":STATE_NONE,},
	2:{"key":12,"name":"CH2","ip":SRV1["host"],"tcp_port":SRV1["ch2"],"udp_port":SRV1["ch2"],"state":STATE_NONE,},
	3:{"key":13,"name":"CH3","ip":SRV1["host"],"tcp_port":SRV1["ch3"],"udp_port":SRV1["ch3"],"state":STATE_NONE,},
	4:{"key":14,"name":"CH4","ip":SRV1["host"],"tcp_port":SRV1["ch4"],"udp_port":SRV1["ch4"],"state":STATE_NONE,},
	5:{"key":15,"name":"CH5","ip":SRV1["host"],"tcp_port":SRV1["ch5"],"udp_port":SRV1["ch5"],"state":STATE_NONE,},
	6:{"key":16,"name":"CH6","ip":SRV1["host"],"tcp_port":SRV1["ch6"],"udp_port":SRV1["ch6"],"state":STATE_NONE,},
}

REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
	}
}

REGION_DICT = {
	0 : {
		1 : { "name" :SRV1["name"], "channel" : SERVER1_CHANNEL_DICT, },
	},
}

MARKADDR_DICT = {
	10 : { "ip" : SRV1["host"], "tcp_port" : SRV1["ch1"], "mark" : "10.tga", "symbol_path" : "10", },
}
