function selectFromChildren(parent, select)
{
	const children = parent.children;
	for(let i = 0; i < children.length; i++)
	{
		children[i].selected = false;
		if(children[i].value == select)
			children[i].selected = true;
	}
}
function setTZSelected()
{
	const tzr1mon = "%R1MON%";
	const tzr2mon = "%R2MON%";
	const tzr1week= "%R1WEEK%";
	const tzr2week= "%R2WEEK%";
	const tzr1dow = "%R1DOW%";
	const tzr2dow = "%R2DOW%";

	const dst_mon = document.getElementById('tzr1mon');
	const std_mon = document.getElementById('tzr2mon');
	const dst_week= document.getElementById('tzr1week');
	const std_week= document.getElementById('tzr2week');
	const dst_dow = document.getElementById('tzr1dow');
	const std_dow = document.getElementById('tzr2dow');

	selectFromChildren(dst_mon, tzr1mon);
	selectFromChildren(std_mon, tzr2mon);
	selectFromChildren(dst_week, tzr1week);
	selectFromChildren(std_week, tzr2week);
	selectFromChildren(dst_dow, tzr1dow);
	selectFromChildren(std_dow, tzr2dow);
}

function setTimezoneTableHidden()
{
	const timezonesettings = document.getElementById('timezonesettings');
	const hide = !(document.getElementById('tzbox').checked);
	timezonesettings.hidden = hide;
}
function setTzrTableHidden()
{
	const tz_table = document.getElementById('timezonetable');
	const utc_table= document.getElementById('utc_table');
	const dst_offs = document.getElementById('dst_offs');
	const std_offs = document.getElementById('std_offs');
	const offs_tr  = document.getElementById('offs_tr');
	const utc_tr   = document.getElementById('utc_offs_tr');
	const button   = document.getElementById('tzsendbtntr');
	const hide   = !(document.getElementById('dstbox').checked);

	tz_table.hidden = hide;
	utc_table.hidden = !hide;

	if(hide)
	{
		utc_tr.appendChild(dst_offs);
		utc_table.appendChild(button);
	}
	else
	{
		offs_tr.insertBefore(dst_offs, std_offs);
		tz_table.appendChild(button);
	}
}

function setNTPManuHidden()
{
	const hide = !(document.getElementById('ntpbox').checked);
	const text = document.getElementById('ntpserv');

	text.disabled = hide;
}

function setBrManuHidden()
{
	const hide =!(document.getElementById('brbox').checked);
	const min_br= document.getElementById('minBrightness');
	const max_br= document.getElementById('maxBrightness');
	const manu  = document.getElementById('manuBrightness');

	min_br.disabled = !hide;
	max_br.disabled = !hide;
	manu.disabled = hide;
}
//Send functions:
function postJson(address, json)
{
	console.log(json);

	fetch(address, {
	method: "POST",
	body: JSON.stringify(json),
	headers: {
		"Content-type": "application/json; charset=UTF-8"
	}
	});
}

function sendDateTime()
{
	const value = document.getElementById('date').value;

	const date = value.split('T')[0];
	const time = value.split('T')[1];
	const year = parseInt(date.split('-')[0]);
	const month= parseInt(date.split('-')[1]);
	const day  = parseInt(date.split('-')[2]);
	const hour = parseInt(time.split(':')[0]);
	const minute=parseInt(time.split(':')[1]);
	let second=parseInt(time.split(':')[2]);

	if(isNaN(second))
		second = 0;

	const json = {
		year: year,
		mon : month,
		day : day,
		hour: hour,
		min : minute,
		sec : second
	};

	postJson("/pref/time", json);
}

function sendTimeZone()
{
	const dst = document.getElementById('dstbox').checked;

	const r1mon  = document.getElementById('tzr1mon').value;	//r1 month
	const r1week = document.getElementById('tzr1week').value;	//r1 week
	const r1dow  = document.getElementById('tzr1dow').value;	//r1 day of week
	const r1h    = document.getElementById('tzr1h').value;		//r1 hour
	const r1offs = document.getElementById('tzr1offs').value;	//r1 offset
	
	const r2mon  = document.getElementById('tzr2mon').value;	//r2 month
	const r2week = document.getElementById('tzr2week').value;	//r2 week
	const r2dow  = document.getElementById('tzr2dow').value;	//r2 day of week
	const r2h    = document.getElementById('tzr2h').value;		//r2 hour
	const r2offs = document.getElementById('tzr2offs').value;	//r2 offset

	let std_json = {
		abbr: "STD",
		mon : r1mon,
		week: r1week,
		dow : r1dow,
		hour: r1h,
		offs: r1offs
	};

	let dst_json;

	if(dst)
	{
		dst_json = {
			abbr: "DST",
			mon : r2mon,
			week: r2week,
			dow : r2dow,
			hour: r2h,
			offs: r2offs
		};
	}
	else //if there is no DST we just send the same rule twice (except the abbr)
	{
		dst_json = {
			abbr: "DST",
			mon : r1mon,
			week: r1week,
			dow : r1dow,
			hour: r1h,
			offs: r1offs
		};
	}

	postJson("/pref/tz/std", std_json);
	postJson("/pref/tz/dst", dst_json);
}

function sendWifi()
{
	const ssid = document.getElementById('wifissid').value;
	const pass = document.getElementById('wifipass').value;

	const json = {
		ssid: ssid,
		pass: pass
	};

	let acknowledge = true;
	if(pass === "")
		acknowledge = confirm("WiFi password is empty. Are you sure you want to send it?");
	if(acknowledge)
		postJson("/pref/wifi", json);
}

function sendNTPserver()
{
	const server = document.getElementById('ntpserv').value;
	const json = {
		ntpserv: server
	};

	postJson("/pref/ntp/serv", json);
}

function sendNTPmanu()
{
	const value = document.getElementById('ntpbox').checked ? 0 : 1;
	const json = {
		ntpmanu: value
	};

	postJson("/pref/ntp/manu", json);
}

function sendNTP()
{
	if(document.getElementById('ntpbox').checked)
		sendNTPserver();
	sendNTPmanu();
}

function sendHclr()
{
	const value = document.getElementById('Hcolor').value;
	const json = {
		hcolor: value
	};

	postJson("/pref/color/h", json);
}

function sendMclr()
{
	const value = document.getElementById('Mcolor').value;
	const json = {
		mcolor: value
	};

	postJson("/pref/color/m", json);
}

function sendSclr()
{
	const value = document.getElementById('Scolor').value;
	const json = {
		scolor: value
	};

	postJson("/pref/color/s", json);
}

function sendBgclr()
{
	const value = document.getElementById('Bgcolor').value;
	const json = {
		bgcolor: value
	};

	postJson("/pref/color/bg", json);
}

function sendMinBrightness()
{
	const value = document.getElementById('minBrightness').value;
	const json = {
		minbright: value
	};

	postJson("/pref/brightness/min", json);
}

function sendMaxBrightness()
{
	const value = document.getElementById('maxBrightness').value;

	const json = {
		maxbright: value
	};

	postJson("/pref/brightness/max", json);
}

function sendManuBrightness()
{
	let value = document.getElementById('manuBrightness').value;

	if(!document.getElementById('brbox').checked)
		value = 0;

	const json = {
		manubright: value
	}

	postJson("/pref/brightness/manu", json);
}

function sendAll()
{
	sendDateTime();
	if(document.getElementById('tzbox').checked)
		sendTimeZone();
	sendWifi();
	sendNTP();
	sendHclr();
	sendMclr();
	sendSclr();
	sendBgclr();
	sendMinBrightness();
	sendMaxBrightness();
	sendManuBrightness();
}

function init()
{
	setTimezoneTableHidden();
	setTzrTableHidden();
	setNTPManuHidden();
	setBrManuHidden();
	setTZSelected();
}
