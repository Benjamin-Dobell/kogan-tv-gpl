#!/bin/bash      
mv ./include/linux/netfilter_ipv4/ipt_ECN.h  ./include/linux/netfilter_ipv4/ipt_ECN_U.h
mv ./include/linux/netfilter_ipv4/ipt_TTL.h ./include/linux/netfilter_ipv4/ipt_TTL_U.h
mv ./include/linux/netfilter_ipv6/ip6t_HL.h ./include/linux/netfilter_ipv6/ip6t_HL_U.h
mv ./include/linux/netfilter/xt_CONNMARK.h ./include/linux/netfilter/xt_CONNMARK_U.h  
mv ./include/linux/netfilter/xt_DSCP.h ./include/linux/netfilter/xt_DSCP_U.h 
mv ./include/linux/netfilter/xt_MARK.h ./include/linux/netfilter/xt_MARK_U.h 
mv ./include/linux/netfilter/xt_RATEEST.h ./include/linux/netfilter/xt_RATEEST_U.h 
mv ./include/linux/netfilter/xt_TCPMSS.h ./include/linux/netfilter/xt_TCPMSS_U.h 
mv ./net/ipv4/netfilter/ipt_ECN.c ./net/ipv4/netfilter/ipt_ECN_U.c
mv ./net/netfilter/xt_DSCP.c ./net/netfilter/xt_DSCP_U.c
mv ./net/netfilter/xt_HL.c ./net/netfilter/xt_HL_U.c
mv ./net/netfilter/xt_RATEEST.c ./net/netfilter/xt_RATEEST_U.c
mv ./net/netfilter/xt_TCPMSS.c ./net/netfilter/xt_TCPMSS_U.c