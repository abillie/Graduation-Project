//********************************************************************//
//! 对象名:Page
//! 功能:定位界面对象，包含界面数据，函数等整个界面功能的实现都在此对象内
//********************************************************************// 
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
    //********************************************************************//
    //! 函数名:getLocation
    //! 功能:通过调用微信api获取当前经纬度和速度，此函数绑定在按钮上，点击按钮时调用
    //! 输入:对象e，调用此函数时传入的对象，包含调用函数的事件信息。此处该函数绑定在界面的按钮上，
    //! 点击按钮产生事件并调用此函数，传入点击事件和点击屏幕位置所在坐标等属性。
    //********************************************************************// 
    getLocation: function (e) {
    console.log("getlocation_e:");
    console.log(e);
    var that = this
    console.log("getlocation_that:");
    console.log(that)
    //! 定时器time2
    var time2 = setInterval(function () {
    console.log("time2:" + time2);
    //! 微信提供的获取经纬度的API
    wx.getLocation({
      //！ 腾讯地图使用火星坐标系，有wgs84和gcj02两种类型，默认返回wgs84
      type: 'gcj02',  
      //！ API调用成功后的回调函数
      success: function (res) {
        // success
        console.log("res:" )
        console.log(res)
        //！ 记录读取到的经纬度和速度
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
    //********************************************************************//
    //! 函数名:openLocation
    //！功能:根据经纬度在地图上显示, bindsubmit="openLocation"，已绑定到表单。
    //! 打开微信自带的地图并跳转到传入的经纬度，此函数绑定在按钮上，点击按钮时调用，并没两秒向服务器上传数据
    //! 输入:对象e，调用此函数时传入的对象，包含调用函数的事件信息，绑定的表单数据（主要时经纬度）等。
    //********************************************************************// 
  openLocation: function (e) {
    console.log("openLocation_e:");   
    console.log(e);
    //! submit事件触发时携带的数据为：e.detail.value
    var value = e.detail.value   
    var that = this
    console.log("value:");
    console.log(value);
    console.log("that:");
    console.log("that:"+that);
    //! 调用微信API打开地图，需传入经纬度
    wx.openLocation({
      longitude: Number(value.longitude),
      latitude: Number(value.latitude)
    })
    //! 定时器time3,定时向服务器上传经纬度数据
 var time3 = setInterval(function () {
      console.log("time3:"+time3);
      //! 调用微信网络请求的API
      wx.request({
       //! 开发环境接口地址
       //! url: 'https://xkmg9m2k.qcloud.la/weapp/location',  
       //！ 提交经纬度的后端服务器地址
        url: 'https://246339601.billie.cc/weapp/location',
        data: {
          longitude: that.data.location.longitude,  //经度
          latitude: that.data.location.latitude,    //纬度
          speed: that.data.speed,
          busflag: that.data.hasLocation
        },
        //! 请求头
        header: {
          'content-type': 'application/json' 
        },
        //! POST方法
        method: "POST",
        //！上传成功并收到服务器回复后的回调函数
        success: function (res) {
          console.log("res.data:"+res.data)
        }
      })
    }, 2000) //! 定时两秒
  },
      //********************************************************************//
    //! 函数名:stoplocation
    //！功能:上传一次经纬度数据，停止两个定时器，停止向服务器自动上传数据
    //! 输入:对象e，调用此函数时传入的对象，包含调用函数的事件信息，绑定的表单数据（主要时经纬度）等。
    //********************************************************************// 
  stoplocation: function (e) {
    clearInterval(time2);
    clearInterval(time3);
    var that=this
    console.log("post_that:"+that)
    //! 调用网络请求API
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
   * 生命周期函数--监听页面隐藏，停止定时器
   */
  onHide: function (e) {
    clearInterval(time2);
    clearInterval(time3);
    stoplocation(e);
  },

  /**
   * 生命周期函数--监听页面卸载，停止定时器
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