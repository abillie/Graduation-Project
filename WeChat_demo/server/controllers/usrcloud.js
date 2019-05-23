
global.Station_flag = false;  //站台状态
global.System_flag = false;   //系统状态
global.temperature = NaN;    //温度
global.humidity = NaN;       //湿度值
global.noise = NaN;          //噪音值
global.pm25 = NaN;          //pm2.5值 
//********************************************************************//
//! 函数名:post
//! 响应数据中转小程序的POST请求，已弃用，改为直接接收站台的GET请求
//! 输入:上下文对象ctx
//! 输出:返回post请求响应结果
//********************************************************************//
async function post(ctx, next) {
  //! 打印post请求数据到日志
  console.log("----------------------admin POST-----------------------------");
  console.log(ctx.request.body);
  Station_flag = ctx.request.body.stationflag;
  console.log("station_flag:" + Station_flag);
  if (Station_flag == true){
    temperature = Number(ctx.request.body.temperature);
    humidity = Number(ctx.request.body.humidity);
    noise = Number(ctx.request.body.noise);
    pm25 = Number(ctx.request.body.pm25);
  }
  if (bus_flag == true && Station_flag == true) {
    System_flag = true;
  }
  console.log("system_flag:" + System_flag);
  console.log("temperature:" + temperature);
  console.log("humidity:" + humidity);
  console.log("noise:" + noise);
  console.log("pm25:" + pm25);
  console.log("bus_flag:" + bus_flag);
  console.log("station_time:" + Station_ArrivalTime);
  console.log("station_number:" + Station_ArrivalStation);
  body = new Object()
  body = {
    busflag: bus_flag,
    systemflag: System_flag,
    stationnumber: Station_ArrivalStation,
    stationtime: Station_ArrivalTime,  
    time: new Date().toLocaleString()
  }
  console.log("body:");
  console.log(body);
  console.log("------------------------admin POST END--------------------------");
  ctx.body = body;
}
//********************************************************************//
//! 函数名:post
//! 响应电子站牌GET请求
//! 输入:上下文对象ctx
//! 输出:返回post请求响应结果
//********************************************************************//
async function get(ctx, next) {
  //! 打印站台的get请求数据到日志	
  console.log("------------------------admin GET --------------------------");
  //! 读取站台单片机上传的数据
  let start_flag = ctx.query.a.charCodeAt();
  //! 转换为ASCII码，再减去48才是实际值，因为单片机传的数据到GPRS模块为字符串
  let station_flag_temp = ctx.query.b.charCodeAt()-48;
  let c = ctx.query.c;
  c = c.charCodeAt();
  let d = ctx.query.d;
  d = d.charCodeAt();
  let e = ctx.query.e;
  e = e.charCodeAt();
  let f = ctx.query.f;
  f = f.charCodeAt();
  let g = ctx.query.g;
  g = g.charCodeAt();
  let h = ctx.query.h;
  h = h.charCodeAt();
  let i = ctx.query.i.charCodeAt();
  //! 打印收到的get数据到日志
  console.log("start_flag:");
  console.log(start_flag);
  console.log("station_flag_temp:");
  console.log(station_flag_temp);
  console.log("c:");
  console.log(c);
  console.log("d:");
  console.log(d);
  console.log("e:");
  console.log(e);
  console.log("f:");
  console.log(f);
  console.log("g:");
  console.log(g);
  console.log("h:");
  console.log(h);
  console.log("i:");
  console.log(i);
  if (station_flag_temp){
    Station_flag = true;
	//! charCodeAt()为字符串转ASCII码，再减48转为实际单片机传入值。
    temperature = ctx.query.c.charCodeAt()-48;
    humidity = ctx.query.d.charCodeAt()-48;
    pm25 = (ctx.query.e.charCodeAt()-48) * 20 + (ctx.query.f.charCodeAt()-48)/5;
    noise = ctx.query.g.charCodeAt()-48 + (ctx.query.h.charCodeAt()-48)/10;
  }
  if (bus_flag == true && Station_flag == true){
    System_flag = true;
  }
  var mytime = new Date();
  //! 获取并打印服务器时间，这个时间要返回单片机
  console.log("day:" + mytime.getDay());
  console.log("data:" + mytime.getDate());
  console.log("hours:" + mytime.getHours());
  console.log("seconds:" + mytime.getSeconds());
  console.log(ctx.query);
  console.log("system_flag:" + System_flag);
  console.log("temperature:" + temperature);
  console.log("humidity:" + humidity);
  console.log("noise:" + noise);
  console.log("pm25:" + pm25);
  console.log("bus_flag:" + bus_flag);
  console.log("station_time:" + Station_ArrivalTime);
  console.log("station_number:" + Station_ArrivalStation);
  console.log("------------------------admin GET END-------------------------");
  //! 响应站台get请求的数据
  let body1 = new Object()
  if (bus_flag == true){
  //! 系统状态	  
  body1.a = 1;     
  }
  else{
  body1.a = 0;    
  }
  if (System_flag == true){
  //! 公交状态	  
  body1.b = 1;      
  }
  else{
  body1.b = 0;
  }
  if (isNaN(Station_ArrivalTime)){
	//! 到达时间  
    body1.c = 0;   
  }
  else{
	//! 到达时间  
    body1.c = Station_ArrivalTime;
  } 
  let temp_distance = Math.ceil(distance_bus_to_station);
  let temp_h = Math.ceil(temp_distance / 100);
  let temp_l = Math.ceil(temp_distance % 100);
  //! 距离前两位
  body1.d = Number(temp_h);
  //! 距离后两位  
  body1.e = Number(temp_l);
  //! 星期几  
  body1.f = mytime.getDay();
  //! 日  
  body1.g = mytime.getDate();
  //! 时
  body1.h = mytime.getHours();
  //! 分
  body1.i = mytime.getMinutes();
  //! 秒
  body1.j = mytime.getSeconds();
  ctx.body = body1;
}
module.exports = {
  post,
  get
}