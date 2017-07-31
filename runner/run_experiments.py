#!/usr/bin/python

import subprocess
import threading
import multiprocessing

conf_str_template = '''init_cwnd: 12
max_cwnd: 15
retx_timeout: 45e-06
queue_size: 36864
propagation_delay: 0.0000002
bandwidth: 40000000000.0
queue_type: {0}
flow_type: 2
num_flow: 100000
flow_trace: CDF_aditya.txt
cut_through: 1
mean_flow_size: 0
load_balancing: 0
preemptive_queue: 0
big_switch: 0
host_type: 1
traffic_imbalance: 0
load: 0.6
reauth_limit: 3
magic_trans_slack: 1.1
magic_delay_scheduling: 1
use_flow_trace: 0
smooth_cdf: 1
burst_at_beginning: 0
capability_timeout: 1.5
capability_resend_timeout: 9
capability_initial: 8
capability_window: 8
capability_window_timeout: 25
ddc: 0
ddc_cpu_ratio: 0.33
ddc_mem_ratio: 0.33
ddc_disk_ratio: 0.34
ddc_normalize: 2
ddc_type: 0
deadline: 0
schedule_by_deadline: 0
avg_deadline: 0.0001
capability_third_level: 1
capability_fourth_level: 0
magic_inflate: 1
interarrival_cdf: none
num_host_types: 13
permutation_tm: 1
srpt_mode: {1}
flow_split_mode: {2}
congestion_compress: {3}
pq_mode: {4}
srpt_with_fair: {5}
'''



template = './simulator 1 conf_{0}_split={1}_compres={2}.txt > result_{0}_split={1}_compres={2}.txt'

def getNumLines(trace):
    out = subprocess.check_output('wc -l {}'.format(trace), shell=True)
    return int(out.split()[0])


def run_exp(rw, semaphore):
    semaphore.acquire()
    print template.format(*rw)
    subprocess.call(template.format(*rw), shell=True)
    semaphore.release()

queue_types = [2, 2, 2, 2, 2, 3, 3, 3, 3, 4]
srpt_mode = [10, 20, 30,30,30, 10, 30, 0, 0, 0]
pq_mode = [0,0,0,0,0,30,30,20,10,0]
srf = [0,0,0,1,2,0,0,0,0,0]
names = ["srpt", "wrong_srpt", "lazy_srpt" , "lazy_srpt_fair", "lazy_srpt_and_wrong",  "srpt_pq", "lazy_srpt_pq", "fifo", "fairness", "sergei_fairness"]

threads = []
semaphore = threading.Semaphore(multiprocessing.cpu_count())

comr_scale = 0.05

for comr in range(1, 11):
  for split_mode in range(5, 6):
    for i in range(0, 10):
      rv = comr_scale * comr;
      conf_str = conf_str_template.format(queue_types[i], srpt_mode[i], split_mode, rv, pq_mode[i], srf[i])
      conf_file = "conf_{0}_split={1}_compres={2}.txt".format(names[i], split_mode, rv)	
      with open(conf_file, 'w') as f:
        print conf_file
        f.write(conf_str)
        threads.append(threading.Thread(target=run_exp, args=((names[i], split_mode, rv), semaphore)))
  


print '\n'
print '\n'
[t.start() for t in threads]
[t.join() for t in threads]
print 'finished', len(threads), 'experiments'
