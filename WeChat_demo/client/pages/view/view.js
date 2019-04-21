// pages/view/view.js
var WxSearch = require('../../wxSearch/wxSearch.js')
Page({
//********************************************************************//
//! 对象名:Page
//! 功能:主界面对象，包含界面数据，函数等整个界面功能的实现都在此对象内
//********************************************************************// 
  data: {
    index: 0,             //选择的站点，最终向服务器请求的数据，服务器根据index返回数据。
    bus_flag: false,      //公交状态
    station_flag: false,  //站台状态
    system_flag: false,   //系统状态
    bus_flag_view: '停运中', //显示站牌状态变量
    system_flag_view: 'OFF!',//显示系统状态变量
    station_number:null,   //距离目的地站台数
    station_time: null,    //达到目的地的时间
    temperature: null,    //温度
    humidity: null,       //湿度值
    noise: null,          //噪音值
    pm25: null,           //pm2.5值    
    distance: null,       //距离  
    Click_flag: true,     //查询事件flag，只点击第一次有效，避免多次点击
    time1: '',
    array: ['明理楼', '香城学府', '院士林', '艺术楼'],
    objectArray: [
      {
        id: 0,
        name: '明理楼'
      },
      {
        id: 1,
        name: '香城学府'
      },
      {
        id: 2,
        name: '院士林'
      },
      {
        id: 3,
        name: '艺术楼'
      }
    ],
  },
  //********************************************************************//
  //! 函数名:bindPickerChange
  //! 功能:记录主界面选择站点的数据，记录选择站点的编号
  //! 输入:对象e，调用此函数时传入的对象，包含调用函数的事件信息。此处该函数绑定在界面的 选择站点 按钮上，
  //********************************************************************// 
  bindPickerChange: function (e) {
    console.log('picker发送选择改变，携带值为', e.detail.value)
    this.setData({
      index: e.detail.value
    })
  },
  /**
   * 生命周期函数--监听页面加载
   * 初始化wxSearch模块数据
   */
  onLoad: function (options) {
    var that =this;
    WxSearch.init(that, 43, ['明理楼', '香城学府', '艺术楼', '院士林']);
    WxSearch.initMindKeys(['明理楼', '香城学府', '院士林', '艺术楼']);
  },
    //********************************************************************//
    //! 函数名:usrcloud
    //! 功能:向服务器请求数据公交车数据，采用get方式。
    //! 输入:对象this，整个界面的全局对象，包含定义的数据等成员。
    //********************************************************************// 
    usrcloud: function(){
    console.log(this);
    var that = this;
    that.setData({
    time1: setInterval(function () { 
    console.log("that:"+that);
    wx.request({
     // url: 'https://xkmg9m2k.qcloud.la/weapp/location', //开发环境接口地址 
      url: 'https://246339601.billie.cc/weapp/location',
      data: {
        station: that.data.index
      },
      header: {
        'content-type': 'application/json' // 默认值
      },
      method: "GET",
      success: function (res) {
        console.log(res.data);
        that.setData({
          bus_flag: res.data.BusFlag,
          system_flag: res.data.SystemFlag,
          station_number: res.data.ArrivalStation,
          station_time: res.data.ArrivalTime,
          distance: res.data.distance,
          temperature: res.data.StationTemperature,
          humidity: res.data.StationHumidity,
          noise: res.data.StationNoise,
          pm25: res.data.StationPm25,
          Click_flag: false
        });
        if (that.data.bus_flag == true){
          that.setData({
            bus_flag_view: '运行中'
          })
        }else{
          that.setData({
          bus_flag_view: '停运中'
          })
        }
        if (that.data.system_flag == true) {
          that.setData({
            system_flag_view: 'OK!'
          })
        } else {
          that.setData({
            system_flag_view: 'OFF!'
          })
        }
      }
    })
  },3000) //定时3秒
    })
  },
  //********************************************************************//
  //! 函数名:Click
  //! 功能:主界面查询按钮绑定的函数，功能是调用一次usrcloud函数
  //! 输入:对象this，调用此函数时传入的对象，包含调用函数的事件信息。此处该函数绑定在界面的 查询 按钮上，
  //********************************************************************// 
  Click: function(){   //限制点击一次
    var that=this;
    console.log(this);
    if (that.data.Click_flag){
      that.usrcloud();
    }
    that.setData({
      Click_flag: false
    })
  },
  //********************************************************************//
  //! 函数名:test
  //! 功能:测试距离计算方法的函数，仅作测试用
  //********************************************************************//
  test: function(){
    let radLat1 = 30.829044 * Math.PI / 180.0;
    let radLat2 = 30.829752 * Math.PI / 180.0;
    let a = radLat1 - radLat2;
    let b = 104.18589 * Math.PI / 180.0 - 104.18362 * Math.PI / 180.0;
    let s = 2 * Math.asin(Math.sqrt(Math.pow(Math.sin(a / 2), 2) + Math.cos(radLat1) * Math.cos(radLat2) * Math.pow(Math.sin(b / 2), 2)));
    s = s * 6378137;
   let distance = Math.round(s * 10000) / 10000;  //单位：米
    console.log(distance);
  },
  //********************************************************************//
  //! 函数名:wxSearchFn
  //! 功能:调用wxSearch模块API的函数，主要实现搜索的 站台名称到index编号的转换
  //! 输入:对象e，调用此函数时传入的对象，包含调用函数的事件信息。此处该函数绑定在界面的 搜索 按钮上，
  //********************************************************************//
  wxSearchFn: function (e) {
    var that = this
    //console.log("wxSearchFn:");
    //console.log(e);
    WxSearch.wxSearchAddHisKey(that);
    if (e.target.detaset == "艺术楼") {
      that.setData({
        index: 3
      })
    }
    if (e.target.detaset == "明理楼") {
      that.setData({
        index: 0
      })
    }
    if (e.target.detaset == "香城学府") {
      that.setData({
        index: 1
      })
    }
    if (e.target.detaset == "院士林") {
      that.setData({
        index: 2
      })
    }
  },
  wxSearchInput: function (e) {
    var that = this
   // console.log("wxSearchInput:");
   // console.log(e);
    WxSearch.wxSearchInput(e, that);
  },
  wxSerchFocus: function (e) {
    //console.log("wxSerchFocus:");
   // console.log(e);
    var that = this
    WxSearch.wxSearchFocus(e, that);
  },
  wxSearchBlur: function (e) {
    var that = this
    //console.log(" wxSearchBlur:");
    //console.log(e);
    WxSearch.wxSearchBlur(e, that);
    if (e.detail.value=="艺术楼"){
      that.setData({
        index: 3
      })
    }
    if (e.detail.value == "明理楼") {
      that.setData({
        index: 0
      })
    }
    if (e.detail.value == "香城学府") {
      that.setData({
        index: 1
      })
    }
    if (e.detail.value == "院士林") {
      that.setData({
        index: 2
      })
    }
  },
  wxSearchKeyTap: function (e) {
    var that = this
    //console.log(" wxSearchKeyTap:");
    //console.log(e);
    if (e.currentTarget.dataset.key == "艺术楼") {
      that.setData({
        index: 3
          
      })
    }
    if (e.currentTarget.dataset.key == "明理楼") {
      that.setData({
        index: 0
      })
    }
    if (e.currentTarget.dataset.key == "香城学府") {
      that.setData({
        index: 1
      })
    }
    if (e.currentTarget.dataset.key == "院士林") {
      that.setData({
        index: 2
      })
    }
    WxSearch.wxSearchKeyTap(e, that);
  },
  wxSearchDeleteKey: function (e) {
    var that = this
    WxSearch.wxSearchDeleteKey(e, that);
  },
  wxSearchDeleteAll: function (e) {
    var that = this;
    WxSearch.wxSearchDeleteAll(that);
  },
  wxSearchTap: function (e) {
    var that = this
    console.log("wxSearchTap");
    console.log(e);
    if (e.target.dataset.key == "艺术楼") {
      that.setData({
        index: 3
      })
    }
    if (e.target.dataset.key == "明理楼") {
      that.setData({
        index: 0
      })
    }
    if (e.target.dataset.key == "香城学府") {
      that.setData({
        index: 1
      })
    }
    if (e.target.dataset.key == "院士林") {
      that.setData({
        index: 2
      })
    }
    WxSearch.wxSearchHiddenPancel(that);

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
    var that=this;
    console.log(that);
    clearInterval(that.data.time1);
    that.setData({
      Click_flag: true
    })
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    var that =this;
    that.setData({
      Click_flag: true
    })
    console.log(that);
    clearInterval(that.data.time1);
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
    var that=this;
    that.setData({
      Click_flag: true
    })
    wx.stopPullDownRefresh();
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