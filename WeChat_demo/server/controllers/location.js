const node_schedule = require('node-schedule') //定时模块
var j = 0;
global.bus_flag = false;      //公交站台
//var station_flag = false;  //站台状态
//var system_flag = false;   //系统状态
//var temperature = NaN;        //温度
//var humidity = NaN;           //湿度值
//var noise = NaN;              //噪音值
//var pm25 = NaN;               //pm2.5值 
var direction = "";               //方向
var direction_temp = "";
global.Station_ArrivalStation = NaN;         //还有几个站点到达,对于固定的公交站
global.Station_ArrivalTime = NaN;             //多少时间后到达，对于固定的公交站
global.distance_bus_to_station = 0;         //公交车到站台距离
var distance_bus_to_man = 0;
var ArrivalStation = NaN;            //还有几个站点到达,对于客户端
var ArrivalTime = 0;               //多少时间后到达，对于客户端
var postData_speed = NaN;            //公交速度
var postData_speed_last = 0;       //上一次公交速度
var distance_sum = NaN;              //站点距目标站点距离
var ArrivalTime_temp = NaN;          //计算时间中间量
var lest = new Array();              //储存距离公交最短的站点数组位置，0，1，2，3
var station_distance = new Array(4);  //各站点之间的距离
var distance_0 = new Array(2);  
var distance_1 = new Array(2);
var distance_2 = new Array(2);
var distance_3 = new Array(2);     //0,1,2,3四个站点距离的中间变量
var longitude_temp = new Array();  //计算方向用经度
var latitude_temp = new Array();   //计算方向用纬度
var distance = new Array(4);       //实时公交与站台之间的距离
 //响应POST请求
 async function post(ctx, next){
  let station_longitude = new Array()  //实时经度
  let station_latitude = new Array()   //实时纬度
  station_longitude[0] = 104.18362
  station_latitude[0] = 30.829752 //明理楼 0
  distance[0] = ""
  station_distance[0] = 308.499;  //01距离

  station_longitude[1] = 104.182945
  station_latitude[1] = 30.827042  //香城学府 1
  distance[1] = ""
  station_distance[1] = 208.7464;  //12距离

  station_longitude[2] = 104.18511  //院士林  2
  station_latitude[2] = 30.826797   
  distance[2] = ""
  station_distance[2] = 261.0112;   //23距离

  station_longitude[3] = 104.18589  //艺术楼 3
  station_latitude[3] = 30.829044
 // station_longitude[3] = 104.185646
 // station_latitude[3] = 30.82834   //艺术楼
  distance[3] = ""
  station_distance[3] = 230.8587;    //30距离

  station_longitude[4] = 104.18589
  station_latitude[4] = 30.829044    //拐角
  //distance[4] = ""
  let postData_longitude = Number(ctx.request.body.longitude);
  let postData_latitude = Number(ctx.request.body.latitude);
  postData_speed = Number(ctx.request.body.speed);
  bus_flag = ctx.request.body.busflag;
  //速度容错处理，计算一段时间内的速度平均值
  if (postData_speed != -1){
    if (postData_speed_last == 0){
      postData_speed_last = postData_speed;
    }else{
      postData_speed_last = (postData_speed + postData_speed_last) / 2;
    }            
  }
  //方向容错处理，保留上一次有效方向
  if (direction != ""){
    direction_temp = direction;
  }
  for (let i=0;i<4;i++)
  {
    let radLat1 = station_latitude[i] * Math.PI / 180.0;
    let radLat2 = postData_latitude * Math.PI / 180.0;
    let a = radLat1 - radLat2;
    let b = station_longitude[i] * Math.PI / 180.0 - postData_longitude * Math.PI / 180.0;
    let s = 2 * Math.asin(Math.sqrt(Math.pow(Math.sin(a / 2), 2) + Math.cos(radLat1) * Math.cos(radLat2) * Math.pow(Math.sin(b / 2), 2)));
    s = s * 6378137;
    distance[i] = Math.round(s * 10000) / 10000;  //单位：米
  }
     //取公交车到各站点距离最小值的数组位置
  function DistanceOfSmallest(a) {
     let lowest = 0;
     for (let n = 1; n < a.length; n++) {
       if (a[n] < a[lowest]) lowest = n;
     }
     return lowest;
   };
   ///if(j != 2){}  //后续增加采样点时间间隔用
   longitude_temp[j] = Number(ctx.request.body.longitude);
   latitude_temp[j] = Number(ctx.request.body.latitude);
   lest[j] = DistanceOfSmallest(distance);
   distance_0[j] = distance[0];  //第j次公交距0，1，2，3站点的距离值。
   distance_1[j] = distance[1];
   distance_2[j] = distance[2];
   distance_3[j] = distance[3];
   j++;
   if(j==2){
    j=0;
    let long_temp = Math.abs(longitude_temp[1] - longitude_temp[0]);
    let lati_temp = Math.abs(latitude_temp[1] - latitude_temp[0]);
    if(lest[0]==lest[1]){
      switch (lest[0]) {
        case 0:
          //if ((distance_0[0] < distance_0[1]) && (long_temp > lati_temp))   //远离0点,方向0~3
           // direction = "03";
          if ((distance_0[0] < distance_0[1]) && (long_temp < lati_temp))   //远离0点，方向0~1
            direction = "01";
          //if ((distance_0[0] > distance_0[1]) && (long_temp < lati_temp))   //靠近0点，方向1~0
           // direction = "10";
          if ((distance_0[0] > distance_0[1]) && (long_temp > lati_temp))   //靠近0点，方向3~0
            direction = "30";
          if (distance_0[0] == distance_0[1])                               //距离0点最近并且保持不动
            direction = "00";
          break;
        case 1:
          if ((distance_1[0] < distance_1[1]) && (long_temp > lati_temp))    //远离1点,方向1~2
            direction = "12";
          //if ((distance_1[0] < distance_1[1]) && (long_temp < lati_temp))    //远离1点，方向1~0
           // direction = "10";
          if ((distance_1[0] > distance_1[1]) && (long_temp < lati_temp))    //靠近1点，方向0~1
            direction = "01";
          //if ((distance_1[0] > distance_1[1]) && (long_temp > lati_temp))    //靠近1点，方向2~1
            //direction = "21";
          if (distance_1[0] == distance_1[1])                                //距离1点最近并且保持不动
            direction = "11";
          break;
        case 2:
         // if ((distance_2[0] < distance_2[1]) && (long_temp > lati_temp))    //远离2点,方向2~1
            //direction = "21";
          if ((distance_2[0] < distance_2[1]) && (long_temp < lati_temp))    //远离2点，方向2~3
            direction = "23";
         // if ((distance_2[0] > distance_2[1]) && (long_temp < lati_temp))    //靠近2点，方向3~2
           // direction = "32";
          if ((distance_2[0] > distance_2[1]) && (long_temp > lati_temp))    //靠近2点，方向1~2
            direction = "12";
          if (distance_2[0] == distance_2[1])                                //距离2点最近并且保持不动
            direction = "22";
          break;
        case 3:
          if ((distance_3[0] < distance_3[1]) && (long_temp > lati_temp))    //远离3点,方向3~0
            direction = "30";
         // if ((distance_3[0] < distance_3[1]) && (long_temp < lati_temp))    //远离3点，方向3~2
            //direction = "32";
          if ((distance_3[0] > distance_3[1]) && (long_temp < lati_temp))    //靠近3点，方向2~3
            direction = "23";
         // if ((distance_3[0] > distance_3[1]) && (long_temp > lati_temp))    //靠近3点，方向0~3
            //direction = "03";
          if (distance_3[0] == distance_3[1])                                //距离3点最近并且保持不动
            direction = "33";
          break;
        default:
          direction = direction_temp; //保持上一次的有效方向
          break;
      }
      //到达两个站点之间的临界点，两次最小距离的站点不同，则公交车远离第一个站，靠近第二个站
      //现阶段仅实现逆时针方向的判断
    } else{  
      switch(lest[1]){
         case 0: 
          direction = "30"
          break;      
         case 1:
          direction = "01"
          break;
         case 2:
          direction = "12"
          break;
         case 3:
          direction = "23"
          //direction = lest.toString();
          break;
        default:
          direction = direction_temp; //保持上一次的有效方向
          break;  
      }
    }
   }
   //计算到达固定站台（明理楼）的到达时间和剩余站点数
   //公交位于01之间时
   if (direction == "01" && postData_speed != NaN) {
     Station_ArrivalStation = 3;
     if (postData_speed != -1) {
       distance_sum = distance[1] + station_distance[1] + station_distance[2] + station_distance[3];
       distance_bus_to_station = distance_sum;
       ArrivalTime_temp = distance_sum / postData_speed;
       Station_ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
     } else {
       Station_ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));   //使用上一次有数据的速度计算，不至于没有值返回。
     }
   };
   //公交位于12之间时
   if (direction == "12" && postData_speed != NaN) {
     Station_ArrivalStation = 2;
     if (postData_speed != -1) {
       distance_sum = distance[2] + station_distance[2] + station_distance[3];
       distance_bus_to_station = distance_sum;
       ArrivalTime_temp = distance_sum / postData_speed;
       Station_ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
     } else {
       Station_ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
     }
   };
   //公交位于23之间时
   if (direction == "23" && postData_speed != NaN) {
     Station_ArrivalStation = 1;
     if (postData_speed != -1) {
       distance_sum = distance[3] + station_distance[3];
       distance_bus_to_station = distance_sum;
       ArrivalTime_temp = distance_sum / postData_speed;
       Station_ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
     } else {
       Station_ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
     }
   };
   //公交位于30之间时
   if (direction == "30" && postData_speed != NaN) {
     Station_ArrivalStation = 1;
     if (postData_speed != -1) {
       distance_sum = distance[0];
       distance_bus_to_station = distance_sum;
       ArrivalTime_temp = distance_sum / postData_speed;
       Station_ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
     } else {
       Station_ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
     }
   };
  console.log("--------------------post---------------------");   
  console.log("j:"+j);
  console.log("longitude_temp:" + longitude_temp);
  console.log("latitude_temp:" + latitude_temp);
  console.log("postData_longitude:"+postData_longitude);
  console.log("postData_latitude:"+postData_latitude);
  console.log("postData_speed:" + postData_speed);
  console.log("ctx.request.body"+ctx.request.body);
  ctx.body = distance;
  console.log("direction:" + direction);
  console.log("(ctx.body:"+ctx.body);
  console.log("--------------------post---------------------");
}
//响应GET请求
async function get(ctx, next){
  let station_temp = Number(ctx.query.station);
  console.log("------------------------get-------------------");
  console.log("station_temp:"+station_temp);
  console.log("direction:" + direction);
  console.log("postData_speed:" + postData_speed);
  switch (station_temp){
      //客户端选择站点0（明理楼）
      case 0:
      //公交位于01之间时
      if (direction == "01" && postData_speed != NaN){  
        ArrivalStation = 3;
        if (postData_speed != -1){
          distance_sum = distance[1] + station_distance[1] + station_distance[2] + station_distance[3];
          distance_bus_to_man = distance_sum;
          ArrivalTime_temp = distance_sum/postData_speed;
          ArrivalTime = Math.ceil(ArrivalTime_temp/60);
          } else{
            ArrivalTime = Math.ceil(distance_sum/(postData_speed_last*60));
          }
        };
        //公交位于12之间时
      if (direction=="12" && postData_speed != NaN) {     
        ArrivalStation = 2;
        if (postData_speed != -1) {
          distance_sum = distance[2] + station_distance[2] + station_distance[3];
          distance_bus_to_man = distance_sum;
          ArrivalTime_temp = distance_sum/postData_speed;
          ArrivalTime = Math.ceil(ArrivalTime_temp/60);
          } else {
            ArrivalTime = Math.ceil(distance_sum/(postData_speed_last * 60));
          }
        };
      //公交位于23之间时
      if (direction=="23" && postData_speed!=NaN) {
        ArrivalStation = 1;
        if (postData_speed != -1) {
          distance_sum = distance[3] +station_distance[3];
          distance_bus_to_man = distance_sum;
          ArrivalTime_temp = distance_sum/postData_speed;
          ArrivalTime = Math.ceil(ArrivalTime_temp/60);
        } else {
          ArrivalTime = Math.ceil(distance_sum/(postData_speed_last * 60));
        }
      };
      //公交位于30之间时
      if (direction=="30" && postData_speed != NaN) {
        ArrivalStation = 1;
        if (postData_speed != -1) {
          distance_sum = distance[0];
          distance_bus_to_man = distance_sum;
          ArrivalTime_temp = distance_sum/postData_speed;
          ArrivalTime = Math.ceil(ArrivalTime_temp/60);
        } else {
          ArrivalTime = Math.ceil(distance_sum/(postData_speed_last * 60));
        }
      };
        break;
       //客户端选择1站点（香城学府）
      case 1:
        //公交位于01之间时
        if (direction == "01" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[1];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于12之间时
        if (direction == "12" && postData_speed != NaN) {
          ArrivalStation = 3;
          if (postData_speed != -1) {
            distance_sum = distance[2] + station_distance[2] + station_distance[3] + station_distance[0];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于23之间时
        if (direction == "23" && postData_speed != NaN) {
          ArrivalStation = 2;
          if (postData_speed != -1) {
            distance_sum = distance[3] + station_distance[3] + station_distance[0];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于30之间时
        if (direction == "30" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[0] + station_distance[0];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        break; 
        //客户端选择2站点（院士林）
      case 2:
        //公交位于01之间时
        if (direction == "01" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[1] + station_distance[1];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于12之间时
        if (direction == "12" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[2];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于23之间时
        if (direction == "23" && postData_speed != NaN) {
          ArrivalStation = 3;
          if (postData_speed != -1) {
            distance_sum = distance[3] + station_distance[3] + station_distance[0] + station_distance[1];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于30之间时
        if (direction == "30" && postData_speed != NaN) {
          ArrivalStation = 2;
          if (postData_speed != -1) {
            distance_sum = distance[0] + station_distance[0] + station_distance[1];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        break;
        //客户端选择站点3（艺术楼）
      case 3:
        //公交位于01之间时
        if (direction == "01" && postData_speed != NaN) {
          ArrivalStation = 2;
          if (postData_speed != -1) {
            distance_sum = distance[1] + station_distance[1] + station_distance[2];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于12之间时
        if (direction == "12" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[2] + station_distance[2];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于23之间时
        if (direction == "23" && postData_speed != NaN) {
          ArrivalStation = 1;
          if (postData_speed != -1) {
            distance_sum = distance[3];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        //公交位于30之间时
        if (direction == "30" && postData_speed != NaN) {
          ArrivalStation = 3;
          if (postData_speed != -1) {
            distance_sum = distance[0] + station_distance[0] + station_distance[1] + station_distance[2];
            distance_bus_to_man = distance_sum;
            ArrivalTime_temp = distance_sum / postData_speed;
            ArrivalTime = Math.ceil(ArrivalTime_temp / 60);
          } else {
            ArrivalTime = Math.ceil(distance_sum / (postData_speed_last * 60));
          }
        };
        break;  
  }
  console.log(" ArrivalStation：" + ArrivalStation + " ArrivalTime：" + ArrivalTime);
  console.log(" distance_sum：" + distance_sum + " ArrivalTime_temp：" + ArrivalTime_temp);
   var body = new Object()
   body.BusFlag = bus_flag;
   body.StationFlag = Station_flag;
   body.SystemFlag = System_flag;
   body.ArrivalStation = ArrivalStation;
   body.ArrivalTime = ArrivalTime;
   body.distance = Math.ceil(distance_bus_to_man);
   body.BusDirection = direction;
   body.StationTemperature = temperature;
   body.StationHumidity = humidity;
   body.StationNoise = noise;
   body.StationPm25 = pm25;
   console.log(body);
   ctx.body = JSON.stringify(body);
  console.log("------------------------get-------------------");
}
module.exports ={
  post,
  get
}