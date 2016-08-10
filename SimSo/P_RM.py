"""
Partitionned EDF using PartitionedScheduler.
"""
from simso.core.Scheduler import SchedulerInfo
from simso.utils import PartitionedScheduler
from simso.schedulers import scheduler

@scheduler("simso.schedulers.P_RM")
class P_RM(PartitionedScheduler):
    def init(self):
        PartitionedScheduler.init(
            self, SchedulerInfo("simso.schedulers.RM_mono"))

    def packer(self):
        # First Fit
        cpus = [[cpu, 0] for cpu in self.processors]
	#task_list.sort(key=lambda x: x.period, reverse=True)
	k = [1]*len(self.processors)
        for task in self.task_list:
	    j = 0
        # Find a processor with free space.
            while cpus[j][1] + float(task.wcet) / task.period > k[j] * (2.0**(1/float(k[j]))-1):
                j += 1
                if j >= len(self.processors):
                    print("oops bin packing failed.")
                    return False

        # Affect it to the task.
            self.affect_task_to_processor(task, cpus[j][0])
            k[j] += 1
        # Update utilization.
            cpus[j][1] += float(task.wcet) / task.period
        return True
