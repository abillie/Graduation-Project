// pages/admin/admin.js
var usrCloud = require('../../utils/usrCloudWx.js');
var md5util = require('../../utils/md5.js');
var token;
var useraccount = 'billie'; //用户名
var password = md5util.hexMD5('usr18780678804'); //密码
var devid = '00010700000000000001';  //设备ID
var nowvalue;
var slaveIndex;
var devOnlineStatus;
var client = new usrCloud();
Page({

  /**
   * 页面的初始数据
   */
  data: {
    status: 0,
    bus_flag: false,      //公交状态
    station_flag: false,  //站台状态
    system_flag: false,   //系统状态
    station_number: null,   //距离目的地站台数
    station_time: null,    //达到目的地的时间
    temperature: null,    //温度
    humidity: null,       //湿度值
    noise: null,          //噪音值
    pm25: null          //pm2.5值 
  },
 
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
  
  },
  //开始连接socket
  openWebSocketConnect: function () {
    var that = this;
    wx.showLoading({
      title: '连接Socket中',
    });
   
    let callback = {
      USR_onConnAck: USR_onConnAck,  //连接回调
      USR_onConnLost: USR_onConnLost, //断开回调
      USR_onRcvParsedDataPointPush: USR_onRcvParsedDataPointPush, //接收将数据点数据解析成json后的回调
      USR_onSubscribeAck: USR_onSubscribeAck, //订阅响应回调
      USR_onUnSubscribeAck: USR_onUnSubscribeAck, //取消订阅响应回调
      USR_onRcvParsedDevStatusPush: USR_onRcvParsedDevStatusPush, //接收解析后上下线推送数据回调
      USR_onRcvParsedDevAlarmPush: USR_onRcvParsedDevAlarmPush, //接收解析后报警数据回调
      USR_onRcvRawFromDev: USR_onRcvRawFromDev,//接收设备原始数据回调，云交换机
      USR_onRcvParsedOptionResponseReturn: USR_onRcvParsedOptionResponseReturn//接收数据发送状态回调
  
    };
    var code = client.Usr_Init('clouddata.usr.cn', 443, 2, callback);//
    console.log(code);
    console.log(useraccount + password);
    var code1 = client.USR_Connect(useraccount, password);//
    console.log("code1:" + code1);
    //连接回调，执行USR_Connect后服务器返回连接成功标志时
    function USR_onConnAck(e) {                        
      console.log("code：" + code + e);
      if (e.code === 0) {
        wx.hideLoading();
        var code4 = client.USR_SubscribeDevRaw(devid);//云交换机，订阅XXX设备推送的原始数据流
        console.log("code4:" + code4 + e);
      }
      console.log('USR_onConnAck', e);
    }
    //连接断开回调，执行USR_DisConnect或异常断开时
    function USR_onConnLost(e) {                       
      console.log('USR_onConnLost', e);
    }
    /*function USR_onRcvParsedDataPointPush(e) {
      var dataInfoList = that.data.dataInfoList;
      var dataPoints = e.dataPoints;
      for (var key in dataPoints) {
        var dataPoint = dataPoints[key];
        var dataInfoList = that.data.dataInfoList;
        for (var key in dataInfoList) {
          if (dataInfoList[key].id == dataPoint.pointId && dataInfoList[key].slaveIndex == dataPoint.slaveIndex) {
            dataInfoList[key].value = dataPoint.value;
            dataInfoList[key].createTime = that.getTime(new Date().getTime() / 1000, 'y-M-d h:m');
          }
        }
      }
      that.setData({ dataInfoList: dataInfoList });
      console.log('USR_onRcvParsedDataPointPush', e);
    }*/
    //订阅回调函数，订阅主题后服务器返回标志时
    function USR_onSubscribeAck(e) {                 
      console.log('USR_onSubscribeAck', e);
    }
    //取消订阅响应回调，取消订阅主题后服务器返回标志时
    function USR_onUnSubscribeAck(e) {               
      console.log('USR_onUnSubscribeAck', e);
    }
    //云交换机 接收设备原始数据回调，接收设备原始数据后服务器返回标志时
    function USR_onRcvRawFromDev(e) {
      console.log('USR_onRcvRawFromDev', e);
      console.log("that:");
      console.log(that);
      that.setData({
        temperature: e.payload[1],
        humidity: e.payload[2],
        pm25: e.payload[3] * 20 + e.payload[4]/5,
        noise: e.payload[5] + e.payload[6]/10,
      });
      console.log("temperature:" + that.data.temperature);
      if (e.payload[0]==255){
        that.setData({
          station_flag: true
        });
        console.log("station_flag:" + that.data.station_flag);
        wx.request({
          //url: 'https://xkmg9m2k.qcloud.la/weapp/usrcloud', //开发环境接口地址 
          url: 'https://246339601.billie.cc/weapp/usrcloud',
          data: {
            stationflag: that.data.station_flag,
            temperature: that.data.temperature,
            humidity: that.data.humidity,
            pm25: that.data.pm25,
            noise: that.data.noise
          },
          header: {
            'content-type': 'application/json' // 默认值
          },
          method: "POST",
          success: function (res) {
            console.log("TRUEres:");
            console.log(res);
            that.setData({
              bus_flag: res.data.busflag,
              system_flag: res.data.systemflag,
              station_number: res.data.stationnumber,
              station_time: res.data.stationtime
            });
          }
        });
      }else{
        wx.request({
         // url: 'https://xkmg9m2k.qcloud.la/weapp/usrcloud', //开发环境接口地址 
          url: 'https://246339601.billie.cc/weapp/usrcloud',
          data: {
            stationflag: that.data.station_flag
          },
          header: {
            'content-type': 'application/json' // 默认值
          },
          method: "POST",
          success: function (res) {
            console.log("ERRORres:");
            console.log(res);

          }
        });
      }
    }
    //云组态 接收设备上下线推送回调，接收设备上下线推送时
    function USR_onRcvParsedDevStatusPush(e) {        
      that.setData({
        status: e.status
      });
      console.log('USR_onRcvParsedDevStatusPush', e);
    }
    //云组态 接收设备报警推送回调，接收设备报警推送
    function USR_onRcvParsedDevAlarmPush(e) {            
      console.log('USR_onRcvParsedDevAlarmPush', e);
    }
    //云组态  接收数据发送状态回调，
    function USR_onRcvParsedOptionResponseReturn(e) {         
      console.log('USR_onRcvParsedOptionResponseReturn', e);
    }
    //云组态 接收数据点变化推送回调，接收到数据点变化推送时
    function USR_onRcvParsedDataPointPush(e) {
      console.log('USR_onRcvParsedDataPointPush', e);
      console.log("string"+client.UTF8ToString(e.payload, 0, e.payload.length));
      console.log("sss" + client.buf2HexStr(e.payload));
    }
  },
  CancelSubscription: function(e){ 
    var code5 = client.USR_UnSubscribeDevRaw(devid); //取消订阅设备原始数据流
    console.log("code5:"+code5);
  },
  closeusr: function(e){
    var code2 = client.USR_DisConnect();
    console.log("code2:" + code2 + e);
  },
  senddata: function(e){
    let that =this;
    let bus =48;
    let system = 48;
    let StationNumber = that.data.station_number + 48;
    let StationTime = that.data.station_time + 48;
    if (that.data.system_flag == true){
      system = 49;
    }
    if (that.data.bus_flag == true) {
      bus = 49;
    } 
    console.log(that);
    var code3 = client.USR_PublishRawToDev(devid, [system, bus, StationNumber, StationTime]);//推送原始数据流，ASCII码形式，48是0的ASCII
    console.log("code3：" + code3+e);
  },
  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
  
  },
  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
  
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
  
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
  
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
  
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
  
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
  
  }
})