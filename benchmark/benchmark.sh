#!/bin/bash
max_server=100
max_client=100
inc=10;

echo ''>result.log
nserver=10;
while [ $nserver -le $max_server ]; do
	nclient=1
	while [  $nclient -le $max_client ]; do		
		msg="num_server:$nserver, num_client:$nclient, iih" 
		echo $msg
		echo $msg >> result.log
		./sim/dmc_sim -s $nserver  -c $nclient | awk '(/mean/ && /std/) || (/average/)' >> result.log		

		#repeat the same with iiv
		msg="num_server:$nserver, num_client:$nclient, iiv"
		echo $msg
		echo $msg >> result.log	
		./sim/dmc_sim -v -s $nserver  -c $nclient | awk '(/mean/ && /std/) || (/average/)' >> result.log	

		let nclient=nclient+inc			
	done
	let nserver=nserver+inc
done


#./sim/dmc_sim -v -s 10  -c 10 | awk '(/mean/ && /std/) || (/average/)' >> result.log

