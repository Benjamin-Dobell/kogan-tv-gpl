#!/bin/bash      
mv ./include/linux/netfilter_ipv4/ipt_ECN_U.h  ./include/linux/netfilter_ipv4/ipt_ECN.h
mv ./include/linux/netfilter_ipv4/ipt_TTL_U.h ./include/linux/netfilter_ipv4/ipt_TTL.h
mv ./include/linux/netfilter_ipv6/ip6t_HL_U.h ./include/linux/netfilter_ipv6/ip6t_HL.h
mv ./include/linux/netfilter/xt_CONNMARK_U.h ./include/linux/netfilter/xt_CONNMARK.h  
mv ./include/linux/netfilter/xt_DSCP_U.h ./include/linux/netfilter/xt_DSCP.h 
mv ./include/linux/netfilter/xt_MARK_U.h ./include/linux/netfilter/xt_MARK.h 
mv ./include/linux/netfilter/xt_RATEEST_U.h ./include/linux/netfilter/xt_RATEEST.h 
mv ./include/linux/netfilter/xt_TCPMSS_U.h ./include/linux/netfilter/xt_TCPMSS.h 
mv ./net/ipv4/netfilter/ipt_ECN_U.c ./net/ipv4/netfilter/ipt_ECN.c
mv ./net/netfilter/xt_DSCP_U.c ./net/netfilter/xt_DSCP.c
mv ./net/netfilter/xt_HL_U.c ./net/netfilter/xt_HL.c
mv ./net/netfilter/xt_RATEEST_U.c ./net/netfilter/xt_RATEEST.c
mv ./net/netfilter/xt_TCPMSS_U.c ./net/netfilter/xt_TCPMSS.c
