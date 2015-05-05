#!/bin/sh
#
# iptable startup script
#
# Allows to start stop restart iptables rules
#


start() {
        echo "Starting iptables"

        #vidage des chaines
        iptables -F
        #destruction des chaines personnelles
        iptables -X

        #stratégies par défaut
        #les connexions entrantes sont rejeté par défaut
        iptables -P INPUT DROP
        #les connexions destinées à être forwardées sont rejeté par défaut
        iptables -P FORWARD DROP
        #les connexions sortantes sont acceptées par défaut
        iptables -P OUTPUT ACCEPT

        # Acceptation de toutes les connexions en local (un process avec l'autre)
        iptables -A INPUT -i lo -j ACCEPT
        iptables -A OUTPUT -o lo -j ACCEPT

        # Acceptation de toutes les connexions en local (un process avec l'autre)
        iptables -A INPUT -i tun0 -j ACCEPT
	iptables -A OUTPUT -o tun0 -j ACCEPT

	## On drop les scans XMAS et NULL.
        iptables -A INPUT -p tcp --tcp-flags FIN,URG,PSH FIN,URG,PSH -j DROP
        iptables -A INPUT -p tcp --tcp-flags ALL ALL -j DROP
        iptables -A INPUT -p tcp --tcp-flags ALL NONE -j DROP
        iptables -A INPUT -p tcp --tcp-flags SYN,RST SYN,RST -j DROP

        ## On Drop les ips TMG
        if [ 1 ]; then
        iptables -N DROP_HADOPI
        iptables -N BAN_HADOPI

        iptables -A BAN_HADOPI -s 85.159.232.0/24  -j DROP_HADOPI
        iptables -A BAN_HADOPI -s 85.159.236.0/24  -j DROP_HADOPI
        iptables -A BAN_HADOPI -s 82.138.74.0/24 -j DROP_HADOPI
        iptables -A BAN_HADOPI -s 82.138.81.0/24  -j DROP_HADOPI
        iptables -A BAN_HADOPI -m iprange --src-range  82.138.70.128-82.138.70.191  -j DROP_HADOPI
        iptables -A BAN_HADOPI -m iprange --src-range  91.189.104.0-91.189.111.255  -j DROP_HADOPI
        iptables -A BAN_HADOPI -m iprange --src-range 193.105.197.0-193.105.197.255 -j DROP_HADOPI
        iptables -A BAN_HADOPI -m iprange --src-range 193.107.240.0-193.107.243.255 -j DROP_HADOPI
        iptables -A BAN_HADOPI -m iprange --src-range 195.191.244.0-195.191.245.255 -j DROP_HADOPI

        iptables -A DROP_HADOPI -j LOG --log-prefix "TridentMedia/Hadopi:   " --log-level warning
        iptables -A DROP_HADOPI -j DROP

        iptables -A INPUT -j BAN_HADOPI
        iptables -A OUTPUT -j BAN_HADOPI
        iptables -A FORWARD -j BAN_HADOPI
        fi

	if [ 1 ]; then
        # Acceptation du ping
        	iptables -N ICMP
		iptables -A ICMP -p icmp -j ACCEPT
		iptables -A INPUT -j ICMP
        fi

        if [ 1 ]; then
        # Acceptation envoi mail
		iptables -A OUTPUT -p tcp --sport 587 -m state --state ESTABLISHED -j ACCEPT
		iptables -A INPUT  -p tcp --dport 587 -j ACCEPT
	        iptables -A INPUT  -p tcp --sport 587 -j ACCEPT
	fi

	# Accepte tout du reseau local
        iptables -A OUTPUT -s 192.168.0.0/24 -j ACCEPT
        iptables -A INPUT -s 192.168.0.0/24 -j ACCEPT
	iptables -A INPUT -p tcp -i eth0 --sport 80 -j ACCEPT

	if [ 1 ]; then
		# Acceptation de l'irc
		iptables -N IRC_I
		iptables -A IRC_I -i eth0 -p tcp --sport 6667 -j ACCEPT 
		iptables -A INPUT -j IRC_I
		
		iptables -N IRC_O
		iptables -A IRC_O -o eth0 -p tcp  --dport 6667 -j ACCEPT
		iptables -A OUTPUT -j IRC_O
	fi

	if [ 1 ]; then
	         # Acceptation de http
		iptables -N HTTPD
		iptables -A HTTPD -i eth0 -p tcp --dport 80 -j ACCEPT
		iptables -A INPUT -j HTTPD
	fi

	if [ 1 ]; then
	         # Acceptation de samba
		iptables -N SAMBA_TUN
		iptables -A SAMBA_TUN -i tun0 -p tcp --dport 445 -j ACCEPT
		iptables -A SAMBA_TUN -i tun0 -p tcp --dport 139 -j ACCEPT
		iptables -A INPUT -j SAMBA_TUN
	fi

       if [ 1 ]; then
                 # Acceptation de openvpn
                iptables -N OPENVPN
                iptables -A OPENVPN -i eth0 -p udp --dport 1194 -j ACCEPT
                iptables -A OPENVPN -i eth0 -p tcp --dport 1194 -j ACCEPT
                iptables -A INPUT -j OPENVPN
        fi

	if [ 1 ]; then
		# Drop le Samba sur reseau interne sans le logguer
		iptables -N SAMBA
		iptables -A SAMBA -p tcp -i eth0 --sport 139 -j DROP
		iptables -A SAMBA -p tcp -i eth0 --sport 443 -j DROP
		iptables -A SAMBA -p tcp -i eth0 --sport 445 -j ACCEPT
		iptables -A SAMBA -p udp -i eth0 --sport 123 -j DROP
		iptables -A INPUT -j SAMBA
	fi


        if [ 1 ]; then
                # Ouverture serveur ssh accesible depuis internet

                # Brute force
                iptables -N SSH_BF
                iptables -A INPUT -p tcp --dport 22 -m state --state NEW -m recent --name SSH --set --rsource -j SSH_BF
                iptables -A SSH_BF -m recent ! --rcheck --seconds 60 --hitcount 5 --name SSH --rsource -j RETURN
                iptables -A SSH_BF -j LOG --log-prefix "SSH Brute Force Attempt:  " --log-level warning
                iptables -A SSH_BF -p tcp -j DROP

                # Lorsque le paquet n'a pas ete droppe: 
                iptables -A INPUT -p tcp --dport 22 -j ACCEPT
        fi
	
	if [ ]; then 
	        #création d'une nouvelle règle
        	iptables -N MAregle
 
	        #définition de la règle : accepter les nouvelles connexions ne venant pas de l'interface internet
	        # et accepter toutes les connexions établies et reliées (ex: une demande de page HTML provoque l'ouverture
	        # d'une connexion reliée pour acheminer cette page vers l'ordinateur)
        	iptables -A MAregle -m state --state NEW ! -i eth0 -j ACCEPT
	        iptables -A MAregle -m state --state ESTABLISHED,RELATED -j ACCEPT

        	#application de la règle au partage de connexion
	        iptables -A INPUT -j MAregle
	        iptables -A FORWARD -j MAregle
	fi
        
	# Permettre à une connexion ouverte de recevoir du trafic en sortie.
        iptables -A OUTPUT -m state ! --state INVALID -j ACCEPT

        # On log les paquets en entrée.
        iptables -A INPUT -j LOG
}


stop() {
        echo "Stopping iptables"
        #vidage des chaines et destruction des chaines personnelles
        iptables -F
        iptables -X

        #stratégies par défaut
        iptables -P INPUT ACCEPT        #les connexions entrantes sont acceptées par défaut
        iptables -P FORWARD ACCEPT      #les connexions destinées à être forwardées sont acceptées par défaut
        iptables -P OUTPUT ACCEPT       #les connexions sortantes sont acceptées par défaut
        # FIN des "politiques par défaut"


}

case "$1" in
start)
        start
        ;;

stop)
        stop
        ;;
restart)
                stop
                start
        ;;
*)
        echo "Usage $0 {start|stop|restart}"
        exit 1
esac

exit 0
