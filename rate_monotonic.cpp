#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <unordered_map>
using namespace std;

int global_time=0;


class executed_process  //PROCESSES that are in the ready queue
{
public:
	int ppid,p_period,startime,deadline,lasttime,pro_time,till_now;
	bool flag;
	executed_process():ppid(0),p_period(1000)
	{

	}
	executed_process(int a,int b,int c) //initiates the instance of given process
	{
		ppid=a;
		p_period=b;
		deadline=c;
	}
	void set_values(int a,int b,int c,int d) //set function
	{
		ppid=a;
		deadline=b;
		p_period=c;
		pro_time=d;
		till_now=0;
		flag=false;
	}
	void increment_till() //time till the process executed
	{
		till_now++;
	}	
};

class task_type //types of process
{
public:
	int pid,processing_time,period,k,k_copy; //information about current process
	executed_process* imp;
	task_type():pid(0),processing_time(0),period(0),k(0)
	{

	}
	task_type(int a,int b,int c,int d)
	 {
	 	set_task(a,b,c,d);
	 	set_imp(d);
	 }
	//~ task_type();
	void set_task(int a,int b,int c,int d) //initiates the instance of given process
	{
		set_pid(a);
		set_processing_time(b);
		set_period(c);
		k=d;
		k_copy=d;
	}
	void set_pid(int a)
	{
		this->pid=a;
	}
	void set_processing_time(int b)
	{
		this->processing_time=b;
	}
	void set_period(int c)
	{
		this->period=c;
	}
	void set_imp(int d)
	{
		imp=new executed_process[d];
	}
	void decrementk() //decrements the counter that how many periods are gone of a given process
	{
		k=k-1;
	}	
};



bool sort_by_period(task_type a,task_type b)  //sorts the periods based on perios
{
	return(a.period<b.period);
	
}

bool sort_by_period_2(executed_process a,executed_process b) //sort the processes in ready queue based on period 
{
	return(a.p_period<b.p_period);

}


void rms_scheduler(vector<task_type>&v,int t,int n) //scheduler that schedules the processes
{
	unordered_map<int ,pair<int,int>>mp; //contains waiting time in second index of pair
	unordered_map<int ,int>turn; //contains turnaround time
	unordered_map<int ,int>dead; //contains the count of instances of given process that missed thear deadline
	for(int i=0;i<n;i++)
	{
		mp[v[i].pid].first=0;
		mp[v[i].pid].second=0;//initialises all the maps key values to 0
		turn[v[i].pid]=0;
		dead[v[i].pid]=0;
	}
	vector<executed_process>ready; //READY QUEUE that contains processes that are waiting for to be executed on CPU
	bool flag_for_idelness=false;
	executed_process extra=executed_process(-99,100000,9000);
	executed_process current;
	current=extra;
	int resumable=0;
	ofstream output,stats,final_output;
	output.open("RM-Log.txt");
	output<<" Data log for "<<n<<" processes\n";
	while(global_time<t)
	{
		if(ready.size()>0)   //this if condition check whether any process in ready queue has missed its deadline
		{
			int p=0;
			for(int i=0;i<ready.size();i++)
			{
				
				if(global_time==ready[i].deadline)
				{
					output<<"process p"<<ready[i].ppid<<" Misses its deadline and thrown out of cpu at t="<<global_time<<"\n";
					turn[ready[i].ppid]+=global_time-mp[ready[i].ppid].first;
					mp[ready[i].ppid].second+=global_time-mp[ready[i].ppid].first;
					dead[ready[i].ppid]++;
					ready[i].p_period=-100;
					p++;
				}
			}
			sort(ready.begin(),ready.end(),sort_by_period_2); //sorts the ready queue after removing the process that missed their deadline
			while(p>0)
			{
				ready.erase(ready.begin());//removes the processe that missed their deadline 
				p--;
			}
		}
		current.flag=true;
		current.increment_till(); //increments the time of current process executing
		for(int i=0;i<n;i++)
		{
			if(global_time % v[i].period==0 && v[i].k>0)//this function checks whether any process is entering in a ready queue at a given nstance
			{
				v[i].decrementk();
				(v[i].imp[v[i].k]).set_values(v[i].pid,global_time+v[i].period,v[i].period,v[i].processing_time);
				ready.push_back(v[i].imp[v[i].k]);
				if(current.ppid==v[i].pid)
					resumable=mp[v[i].pid].first;
				mp[v[i].pid].first=global_time;
				output<<"process p"<<v[i].pid<<" joined the system at  t="<<global_time<<"\n";
			}
		}
		sort(ready.begin(),ready.end(),sort_by_period_2);//sorts after adding the processses in readdy queue
		//this if condition checks whether the proces sin ready queue has higher priority then the one currently running
		if( ready.size()>0 && current.p_period>ready[0].p_period && current.till_now!=current.pro_time)
		{
			if(flag_for_idelness==true)
			{
				output<<"idle till t="<<global_time<<"\n";
				flag_for_idelness=false;
			}
			executed_process p=ready[0];
			ready.erase(ready.begin());
			if(current.ppid!=-99) //current process gets preemoted
			{
				turn[current.ppid]+=global_time-mp[current.ppid].first;
				mp[current.ppid].first=global_time;
				ready.push_back(current);
				output<<"process p"<<current.ppid<<" preempted by process p"<<p.ppid<<".Remaining execution time="<<current.pro_time-current.till_now<<"\n";
			}
			current=p;
			mp[current.ppid].second+=global_time-mp[current.ppid].first;
			output<<"process p"<<current.ppid<<" starts execution at t="<<((global_time>0)? (global_time+1):(global_time))<<"\n";
			sort(ready.begin(),ready.end(),sort_by_period_2);//again sorts the processes in ready queue	
		}
		//this if condition checks whether the current process has completed its execution or not  
		if(current.till_now==current.pro_time &&current.ppid!=-99)
		{
			//takes out the first process from ready
			output<<"process p"<<current.ppid<<" finishes execution at t="<<global_time<<"\n";
			turn[current.ppid]+=global_time-mp[current.ppid].first;
			if(ready.size()!=0) 
			{
				current=ready[0];
				ready.erase(ready.begin());
				if(current.flag==true) //resumable process comes out of ready queue
					output<<"process p"<<current.ppid<<" resumes execution at t="<<global_time+1<<"\n";
				else//new process instance comes in
					output<<"process p"<<current.ppid<<" starts execution at t="<<global_time+1<<"\n";
				mp[current.ppid].second+=global_time-mp[current.ppid].first;
			}
			else
			{
				current=extra;
				flag_for_idelness=true;
			}
			
		}

		//this if function checks whether the current running process has missed its deadline or not
		if(current.deadline<=global_time && current.till_now<current.pro_time && current.ppid!=-99)
		{
			//takes out hte process from the current
			dead[current.ppid]++;
			if(mp[current.ppid].first==global_time)
				turn[current.ppid]+=global_time-resumable;
			else
				turn[current.ppid]+=global_time-mp[current.ppid].first;
			output<<"process p"<<current.ppid<<" misses its deadline and thrown out of cpu at t="<<global_time<<"\n";
			if(ready.size()!=0)
			{
				current=ready[0];
				ready.erase(ready.begin());
				if(current.flag==true)   //resumable process comes out of ready queue
					output<<"process p"<<current.ppid<<" resumes execution at t="<<global_time+1<<"\n";
				else     //resumable process comes out of ready queue
					output<<"process p"<<current.ppid<<" starts execution at t="<<global_time+1<<"\n";
				mp[current.ppid].second+=global_time-mp[current.ppid].first;
			}
			else
			{
				current=extra;
				flag_for_idelness=true;
			}
		}
		global_time++;
	}
	output.close();
	stats.open("RM-Stats.txt");
	stats<<" Data log for "<<n<<" processes\n";
	int total_process=0,completed=0,dead_t=0,total_wait=0,total_turn=0;
	for(int i=0;i<n;i++)   //calculates the total number of process that completes and missed their deadlines 
	{
		total_process+=v[i].k_copy;
		completed+=v[i].k_copy-dead[v[i].pid];
		dead_t+=dead[v[i].pid];
	}
	stats<<"Number of processes that came into the system="<<total_process<<"\n";
	stats<<"Number of processes that completed successfully="<<completed<<"\n";
	stats<<"Number of processes that missed their deadlines="<<dead_t<<"\n";

	for(int i=0;i<n;i++) //calculates the total waiting and turnaround time for each process
	{
		stats<<"Waiting time for process p"<<v[i].pid<<"= "<<mp[v[i].pid].second<<"\n";
		stats<<"Turnaround time for process p"<<v[i].pid<<"= "<<turn[v[i].pid]<<"\n";
		total_wait+=mp[v[i].pid].second;
		total_turn+=turn[v[i].pid];
	}
	stats.close(); //outputs to the file
    final_output.open("Average_time.txt", std::ios_base::app);
  	final_output<<"\n\nAverage times for RMS scheduling with "<<n<<" processes\n";
  	final_output<<"Average Waiting time = "<<total_wait/n<<"\n";
  	final_output<<"Average Turnaround time = "<<total_turn/n<<"\n";
  	final_output.close();
	cout<<"\nProgram Executed successfully please see the output files for results\n";


}


int main()
{
	int n,max=0;
	ifstream myfile;
	myfile.open("a.txt"); //read the input from file a.txt
	myfile>>n;
	vector<task_type>Tasks;
	unordered_map<int, pair<int ,int >> mp;
	for(int i=0;i<n;i++)
	{
		int a,b,c,d;
		myfile>>a>>b>>c>>d;
		if(max<c*d)
			max=c*d;
		Tasks.push_back(task_type(a,b,c,d));
	}
	sort(Tasks.begin(),Tasks.end(),sort_by_period); //sorts the tasks by their period
	//cout<<max;
	rms_scheduler(Tasks,max+10,n);//calls the rms_scheduler function
	return 0;
}