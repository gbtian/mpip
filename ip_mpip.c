#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE

#include <linux/module.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/sysctl.h>

#include <net/ip_mpip.h>


int sysctl_mpip_enabled __read_mostly = 1;

static struct ctl_table mpip_table[] = {
	{
		.procname = "mpip_enabled",
		.data = &sysctl_mpip_enabled,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = &proc_dointvec
	},
	{ }
};

/*
 * 	Process ingress packets with mpip mechanism
 */
int mpip_rcv(struct sk_buff *skb)
{
	int i;
	struct ethhdr *eth;

	eth = eth_hdr(skb);

	printk("src mac:");
	for(i = 0; i < ETH_ALEN; i++)
		printk(KERN_ALERT "%02x", eth->h_source[i]);
	printk("\n");

	printk("dest mac:");
	for(i = 0; i < ETH_ALEN; i++)
		printk(KERN_ALERT "%02x", eth->h_dest[i]);

	return 0;
}


void mpip_options_build(struct sk_buff *skb)
{
	struct mpip_options *opt;
	unsigned char	node_id[ETH_ALEN] = {'a', 'b', 'c', 'd', 'f', 'g'};
	opt = kmalloc(sizeof(struct mpip_options), GFP_KERNEL);

	opt->optlen = sizeof(struct mpip_options);
	memcpy(opt->node_id, node_id, ETH_ALEN);
	opt->session_id = 8;
	opt->path_id = 3;
	opt->stat_path_id = 4;
	opt->packetcount = 500;

	//todo: the value of option will be extracted from all the tables.

	unsigned char *iph = skb_network_header(skb);

	memcpy(&(IPCB(skb)->opt), opt, sizeof(struct mpip_options));
	memcpy(iph+sizeof(struct iphdr), opt->__data, opt->optlen);
}
EXPORT_SYMBOL(mpip_options_build);

bool mpip_rcv_options(struct sk_buff *skb)
{
	struct mpip_options *opt;
	const struct iphdr *iph;
	struct net_device *dev = skb->dev;

	iph = ip_hdr(skb);
	opt = &(IPCB(skb)->opt);

	//todo: process the options

	return true;
}
EXPORT_SYMBOL(mpip_rcv_options);


void mpip_undo(void)
{
}

static int __init mpip_init(void)
{
	struct ctl_table_header *mptcp_sysctl;
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "enter mpip module!\n");
    printk(KERN_ALERT "*************************\n");

    //In kernel, __MPIP__ will be checked to decide which functions to call.
	mptcp_sysctl = register_net_sysctl(&init_net, "net/mpip", mpip_table);
	if (!mptcp_sysctl)
		goto register_sysctl_failed;

//    witable = kmalloc(sizeof(struct working_ip_table), GFP_KERNEL);
//    INIT_LIST_HEAD(&witable->list);
//
//    pitable = kmalloc(sizeof(struct path_info_table), GFP_KERNEL);
//    INIT_LIST_HEAD(&pitable->list);
//
//    sstable = kmalloc(sizeof(struct sender_session_table), GFP_KERNEL);
//    INIT_LIST_HEAD(&sstable->list);
//
//    rstable = kmalloc(sizeof(struct receiver_socket_table), GFP_KERNEL);
//    INIT_LIST_HEAD(&rstable->list);
//
//    pstable = kmalloc(sizeof(struct path_stat_table), GFP_KERNEL);
//    INIT_LIST_HEAD(&pstable->list);

	register_sysctl_failed:
		mpip_undo();

    return 0;
}

static void __exit mpip_exit(void)
{

    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "exit mpip module!\n");
    printk(KERN_ALERT "*************************\n");
}

module_init(mpip_init);
module_exit(mpip_exit);
