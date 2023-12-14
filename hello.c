
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Oleksii Chyzhov <chyzhov.oleksii@lll.kpi.ua>");
MODULE_DESCRIPTION("'Hello, world' module, Lab 5");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int hello_count = 1;

module_param(hello_count, uint, S_IRUGO);
MODULE_PARM_DESC(hello_count, "Number of times the message should be printed");

struct my_list_entry {
	struct list_head list_node;
	ktime_t timestamp;
};

static LIST_HEAD(my_list_head);

static int __init hello_init(void)
{
	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
		pr_warn("The parameter value is 0 or between 5 and 10.\n");
	}

	BUG_ON(hello_count > 10);

	while (hello_count > 0) {
		struct my_list_entry *ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);

		BUG_ON(!ptr);

		INIT_LIST_HEAD(&ptr->list_node);
		ptr->timestamp = ktime_get();
		list_add(&ptr->list_node, &my_list_head);
		printk(KERN_EMERG "Hello, world!\n");

		if (hello_count == 2) {
			// Введення примусової помилки для останнього повідомлення в серії
			pr_err("Simulating kmalloc() returning 0 for the last message in the series.\n");
			kfree(ptr);
			BUG_ON(1);
		}

		hello_count--;
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct my_list_entry *ptr;
	struct my_list_entry *tmp;

	list_for_each_entry_safe(ptr, tmp, &my_list_head, list_node) {
		printk(KERN_EMERG "Event timestamp: %lld\n", ptr->timestamp);
		list_del(&ptr->list_node);
		kfree(ptr);
	}
}

module_init(hello_init);
module_exit(hello_exit);
