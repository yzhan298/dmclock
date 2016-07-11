#!/usr/bin/env python


class DataPoint: 
        def __init__(self):                
                self.nserver = 0;
                self.nclient = 0;
                self.heap_type = 0;
                self.cl_track_response = 0
                self.cl_req_param = 0

                self.sr_add_req_time_mean = 0;
                self.sr_add_req_time_std = 0;
                self.sr_add_req_time_low = 0;
                self.sr_add_req_time_high = 0;

                self.sr_req_complete_time_mean = 0;
                self.sr_req_complete_time_std = 0;
                self.sr_req_complete_time_low = 0;
                self.sr_req_complete_time_high = 0;
                
                self.sr_total_time_to_add_req = 0;
                self.sr_total_time_to_complete_req = 0;
                
        def __str__(self):
                return "%d %d"%(self.nserver, self.nclient);


def isFloat(elem):        
        try:
                float(elem)
                return True
        except ValueError:
                return False
#end isFloat


# end DataPoint
dps = [];
dp = None;
state = 1;
with open('result.log', 'r') as f:
	for line in f:
		line = line.strip('\n \t')
		if not line: continue;
		
		if line.endswith("iih") or line.endswith("iiv"):
		        if dp: 
		                dps.append(dp);
		                state = 1;
	                
		        dp = DataPoint();
		        r = [int(s.strip().split(':')[-1]) for s in line.split(',') if len(s.strip().split(':'))>1 ]
		        dp.nserver = r[0];
                        dp.nclient = r[-1];
                        dp.heap_type = 1 if line.endswith("iih") else 0;
                        state += 1
		        
	        elif line.startswith("average"):	
	                #print line                
	                r = [float(s) for s in line.split(' ') if isFloat(s)]
	                #print r #if isFloat(s)
                        if state == 2:
                                dp.cl_track_response = r[0]
                                state +=1;                
                        elif state == 3:
                                dp.cl_req_param = r[0]
                                state +=1;
                        elif state == 6:
                                dp.sr_total_time_to_add_req = r[0];
                                state +=1;
                        elif state == 7:                        
                                dp.sr_total_time_to_complete_req = r[0];
                                state +=1;
                        else: pass
                        
                elif line.startswith("Server add_request_timer"):
                        r = [float(s.strip().split(':')[-1]) for s in line.split(',') if len(s.strip().split(':'))>1 ]
                        #print r
                        dp.sr_add_req_time_mean = r[1];
                        dp.sr_add_req_time_std = r[2];
                        dp.sr_add_req_time_low = r[3];
                        dp.sr_add_req_time_high = r[4];
                        state += 1; #5
                        
                elif line.startswith("Server request_complete_timer"):
                        r = [float(s.strip().split(':')[-1]) for s in line.split(',') if len(s.strip().split(':'))>1 ]
                        dp.sr_req_complete_time_mean = r[1];
                        dp.sr_req_complete_time_std = r[2];
                        dp.sr_req_complete_time_low = r[3];
                        dp.sr_req_complete_time_high = r[4];
                        state += 1; #6

                else: 
                        pass;
        dps.append(dp)
                                
fields = ['nserver', 'nclient', \
        'cl_track_response_h', 'cl_track_response_v', \
        'cl_req_param_h',  'cl_req_param_v', \
        'sr_add_req_time_mean_h', 'sr_add_req_time_mean_v',\
        'sr_add_req_time_std_h', 'sr_add_req_time_std_v', \
        'sr_add_req_time_low_h', 'sr_add_req_time_low_v', \
        'sr_add_req_time_high_h', 'sr_add_req_time_high_v', \
        'sr_req_complete_time_mean_h', 'sr_req_complete_time_mean_v',\
        'sr_req_complete_time_std_h', 'sr_req_complete_time_std_v',\
        'sr_req_complete_time_low_h', 'sr_req_complete_time_low_v',\
        'sr_req_complete_time_high_h', 'sr_req_complete_time_high_v',\
        'sr_total_time_to_add_req_h', 'sr_total_time_to_add_req_v',\
        'sr_total_time_to_complete_req_h', 'sr_total_time_to_complete_req_v'];

with open('result.cvs', 'w+') as f:
        first = True;
        for field in fields:                
                if first:
                        f.write(field);
                        first = False;
                else:
                        f.write(', %s'%(field))
        f.write('\n');
        
        print "total pairs: ", len(dps)
        for i in range(len(dps)/2):
        	dp0 = dps[2*i];
        	dp1 = dps[2*i + 1]
        	f.write("%d, %d,"%(dp0.nserver, dp0.nclient));
        	f.write("%f, %f,"%(dp0.cl_track_response, dp1.cl_track_response));
                f.write("%f, %f,"%(dp0.cl_req_param, dp1.cl_req_param));
                f.write("%f, %f,"%(dp0.sr_add_req_time_mean, dp1.sr_add_req_time_mean));
                f.write("%f, %f,"%(dp0.sr_add_req_time_std, dp1.sr_add_req_time_std));
                f.write("%f, %f,"%(dp0.sr_add_req_time_low, dp1.sr_add_req_time_low));
                f.write("%f, %f,"%(dp0.sr_add_req_time_high, dp1.sr_add_req_time_high));
                f.write("%f, %f,"%(dp0.sr_req_complete_time_mean, dp1.sr_req_complete_time_mean));
                f.write("%f, %f,"%(dp0.sr_req_complete_time_std, dp1.sr_req_complete_time_std));
                f.write("%f, %f,"%(dp0.sr_req_complete_time_low, dp1.sr_req_complete_time_low));
                f.write("%f, %f,"%(dp0.sr_req_complete_time_high, dp1.sr_req_complete_time_high));
                f.write("%f, %f,"%(dp0.sr_total_time_to_add_req, dp1.sr_total_time_to_add_req));
                f.write("%f, %f,"%(dp0.sr_total_time_to_complete_req, dp1.sr_total_time_to_complete_req));
                f.write('\n');
                print 'processing pair', i;
