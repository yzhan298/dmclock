#!/usr/bin/env python
output_file="data.log"

class DataPoint:  
  def __init__(self):                
    self.nserver = 0;
    self.nclient = 0;
    self.heap_type = 0;  
    self.total_time_to_add_req = 0;
    self.total_time_to_complete_req = 0;
    self.config = ''

  def set_name(self, config, heap_type):
    self.config = config;
    self.heap_type = heap_type

  def get_conig(self):
    import re
    return re.split(r"/|\.", self.config)[1]

  def __str__(self):
    return "s:%d, c:%d,h:%d,config:%s"%(self.nserver, self.nclient, self.heap_type, self.config);

# end DataPoint

def isFloat(elem):        
 try:
  float(elem)
  return True
 except ValueError:
  return False
#end isFloat


def parse_config_params(fname):
  nclient = 0;
  nserver = 0;
  # read config file property 
  with open(fname, 'r') as f:
    for line in f:
      line = line.strip('\n \t')
      if not line: continue;
      if line.startswith("client_count"):
        nclient += int(line.split('=')[-1]);
      if line.startswith("server_count"): 
        nserver += int(line.split('=')[-1]);
  # end of file
  return [nserver, nclient];
# parse_config_params

def calc_average(dps, config, heap_type): 
    dp = DataPoint();
    dp.set_name(config, heap_type); 
    num_run = 0
    for _dp in dps:
      if _dp.config == config and _dp.heap_type == heap_type:
        # print _dp, config, heap_type
        dp.nserver =_dp.nserver
        dp.nclient = _dp.nclient
        num_run                       += 1
        dp.total_time_to_add_req      += _dp.total_time_to_add_req
        dp.total_time_to_complete_req += _dp.total_time_to_complete_req 
        
    # average
    dp.total_time_to_add_req      /= num_run;
    dp.total_time_to_complete_req /= num_run
    #print dp
    return dp;

def parse_data_points(filename):
  dps = []; #data-points
  dp = None;
  state = 0;
  configs = {}
  
  with open(filename, 'r') as f:
    for line in f:
      line = line.strip('\n \t')
      if not line: continue;

      if line.startswith("file_name"):      
        if dp:
          dps.append(dp);
          state = 0;
         
        # new data-point
        dp = DataPoint();
        parts = line.split(':')
        fname = parts[-1];        
        dp.heap_type = 1 if parts[1].startswith('heap') else 0;
        
        # add to the dictionary
        configs[fname] = 1;
        
        dp.config = fname;  
        params = parse_config_params(fname)      
        dp.nserver = params[0];
        dp.nclient = params[-1];
         
      elif line.startswith("average"):	# take last 2 averages
        r = [float(s) for s in line.split(' ') if isFloat(s)]
        state +=1;
        #print r, dp #if isFloat(s)
        if state == 3:
          dp.total_time_to_add_req = r[0]
        elif state == 4:
          dp.total_time_to_complete_req = r[0]
        else: pass

      else: 
        pass;    
  # final entry
  dps.append(dp) 
  
  # compute average of multiple runs
  dps_avg = []
  for config in configs:
    dp_heap = calc_average(dps, config , 1);
    dps_avg.append(dp_heap);

    dp_pq   = calc_average(dps, config , 0);    
    dps_avg.append(dp_pq);
          
  return dps_avg;
  
# end parse_data_points


def make_data(filename):
  fields = ['nserver_nclient(config_file)', \
            'add_req_h', 'add_req_pq',\
            'complete_req_h', 'complete_req_pq'];

  dps = parse_data_points(filename);
  with open('%s.dat'%(filename), 'w+') as f:
    first = True;
    for field in fields:                
      if first:
        f.write(field);
        first = False;
      else:
        f.write(' %s'%(field))
    f.write('\n');
    
    print "total pairs: ", len(dps)
    for i in range(len(dps)/2):
    	dp0 = dps[2*i];
    	dp1 = dps[2*i + 1]
    	f.write("s:%d_c:%d "%(dp0.nserver, dp0.nclient));
    	f.write("%f %f "%(dp0.total_time_to_add_req, dp1.total_time_to_add_req));
    	f.write("%f %f "%(dp0.total_time_to_complete_req, dp1.total_time_to_complete_req));
    	f.write('\n');
    	print 'processing pair', i;


def main():
  make_data(output_file);

if __name__ == "__main__":
  main()

