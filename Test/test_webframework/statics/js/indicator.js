function updateData(){
	  $.ajax({
        url:"/indicator",
        dataType:"json",
        success:function(res) {
            document.getElementById('usedmemory').innerHTML = res.used_memory + 'K';
			document.getElementById('totalkeys').innerHTML = res.total_keys;
			document.getElementById('clients').innerHTML = res.client_count;
			document.getElementById('cmdprocessed').innerHTML = res.cmd_processed;
			document.getElementById('uptime').innerHTML = res.uptime + 's';
        }
    });	

	console.log('timer');
}

var timer;
clearInterval(timer);
timer = setInterval( updateData, 1000 );