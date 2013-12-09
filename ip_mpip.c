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

 /*
  * 	Process egress packets with mpip mechanism
  */
int	mpip_xmit(struct sk_buff *skb)
{
	return 0;
}


void mpip_options_build(struct sk_buff *skb, struct mpip_options *opt)
{
	unsigned char *iph = skb_network_header(skb);

	memcpy(&(IPCB(skb)->opt), opt, sizeof(struct mpip_options));
	memcpy(iph+sizeof(struct iphdr), opt->__data, opt->optlen);
	opt = &(IPCB(skb)->opt);

}


static inline bool mpip_rcv_options(struct sk_buff *skb)
{
	struct mpip_options *opt;
	const struct iphdr *iph;
	struct net_device *dev = skb->dev;

	iph = ip_hdr(skb);
	opt = &(IPCB(skb)->opt);

	//todo: process the options

	return true;
}



unsigned char *form_mpip_options_string(mpip_options *opt)
{
	unsigned char *mpip_options;
	unsigned char	session_id = 8;
	unsigned char	path_id = 3;
	unsigned char	stat_path_id = 4;
	u32		packetcount = 500;

	unsigned char	node_id[ETH_ALEN] = {'a', 'b', 'c', 'd', 'f', 'g'};
	mpip_options = kmalloc(ETH_ALEN * 2 + 1, GFP_KERNEL);
	memcpy(mpip_options, node_id, ETH_ALEN);


    printk(KERN_ALERT "node_id = %s\n", node_id);
    printk(KERN_ALERT "session_id = %d\n", session_id);
    printk(KERN_ALERT "path_id = %d\n", path_id);
    printk(KERN_ALERT "stat_path_id = %d\n", stat_path_id);
    printk(KERN_ALERT "packetcount = %d\n", packetcount);

	mpip_options[ETH_ALEN] = session_id;
	mpip_options[ETH_ALEN + 1] = path_id;
	mpip_options[ETH_ALEN + 2] = stat_path_id;
	mpip_options[ETH_ALEN + 3] = ((unsigned char *)& packetcount)[0];
	mpip_options[ETH_ALEN + 4] = ((unsigned char *)& packetcount)[1];
	mpip_options[ETH_ALEN + 5] = ((unsigned char *)& packetcount)[2];
	mpip_options[ETH_ALEN + 6] = ((unsigned char *)& packetcount)[3];

    printk(KERN_ALERT "mpip_options = %s\n", mpip_options);

	return mpip_options;
}


void extract_mpip_options(unsigned char *mpip_options)
{
	unsigned char	node_id[ETH_ALEN];
	unsigned char	session_id;
	unsigned char	path_id;
	unsigned char	stat_path_id;
	u32		packetcount;

    printk(KERN_ALERT "1mpip_options = %s\n", mpip_options);

	memcpy(node_id, mpip_options, ETH_ALEN);
	session_id = mpip_options[ETH_ALEN];
	path_id = mpip_options[ETH_ALEN + 1];
	stat_path_id = mpip_options[ETH_ALEN + 2];

	packetcount = (mpip_options[ETH_ALEN + 3] << 24) |
			(mpip_options[ETH_ALEN + 4] << 16) |
			(mpip_options[ETH_ALEN + 5] << 8) |
			mpip_options[ETH_ALEN + 6];

    printk(KERN_ALERT "1node_id = %s\n", node_id);
    printk(KERN_ALERT "1session_id = %d\n", session_id);
    printk(KERN_ALERT "1path_id = %d\n", path_id);
    printk(KERN_ALERT "1stat_path_id = %d\n", stat_path_id);
    printk(KERN_ALERT "1packetcount = %d\n", packetcount);

    kfree(mpip_options);

}

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
