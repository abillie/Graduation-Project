// location/location.js
//var app = getApp()

Page({

  /**
   * 页面的初始数据
   */
  data: {
      speed: '',
      postdata: 1,
      //默认未获取地址
      hasLocation: false
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
 
  },

    getLocation: function (e) {
    console.log("getlocation_e:");
    console.log(e);
    var that = this
    console.log("getlocation_that:");
    console.log(that)
    var time2 = setInterval(function () {
      console.log("time2:" + time2);
    wx.getLocation({
      type: 'gcj02',  //火星坐标系，有wgs84和gcj02两种类型，默认返回wgs84
      success: function (res) {
        // success
        console.log("res:" )
        console.log(res)
        that.setData({
          hasLocation: true,
          location: {
            longitude: res.longitude,
            latitude: res.latitude
          },
          speed: res.speed
        })
      }
    })
    },2000)//两秒一次循环获取地址经纬度
  },

  //根据经纬度在地图上显示 bindsubmit="openLocation"，已绑定到表单
  openLocation: function (e) {
    console.log("openLocation_e:");   
    console.log(e);
    var value = e.detail.value   //submit事件触发时携带的数据为：e.detail.value
    var that = this
    console.log("value:");
    console.log(value);
    console.log("that:");
    console.log("that:"+that);
    wx.openLocation({
      longitude: Number(value.longitude),
      latitude: Number(value.latitude)
    })
 var time3 = setInterval(function () {
      console.log("time3:"+time3);
      wx.request({
       // url: 'https://xkmg9m2k.qcloud.la/weapp/location', //开发环境接口地址 
        url: 'https://246339601.billie.cc/weapp/location',
        data: {
          longitude: that.data.location.longitude,  //经度
          latitude: that.data.location.latitude,    //纬度
          speed: that.data.speed,
          busflag: that.data.hasLocation
        },
        header: {
          'content-type': 'application/json' // 默认值
        },
        method: "POST",
        success: function (res) {
          console.log("res.data:"+res.data)
        }
      })
    }, 2000)
  },
  //post上传位置
  stoplocation: function (e) {
    clearInterval(time2);
    clearInterval(time3);
    var that=this
    console.log("post_that:"+that)
 //   var time4 = setInterval(function () {
    wx.request({
      //url: 'https://xkmg9m2k.qcloud.la/weapp/location', //开发环境接口地址 
      url: 'https://246339601.billie.cc/weapp/location',
      data: {
        longitude: that.data.location.longitude,  //经度
        latitude: that.data.location.latitude,    //纬度
        speed: that.data.speed,
        busflag: false
      },
      header: {
        'content-type': 'application/json' // 默认值
      },
      method: "POST",
      success: function (res) {
        console.log(res.data)
      }
    })
 //   }, 2000)
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
  onHide: function (e) {
    clearInterval(time2);
    clearInterval(time3);
    stoplocation(e);
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function (e) {
    clearInterval(time2);
    clearInterval(time3);
    stoplocation(e);
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