// pages/access/access.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    account: '',
    password: ''
  },
  // 获取输入账号
  phoneInput: function (e) {
    this.setData({
      account: e.detail.value
    })
  },

  // 获取输入密码
  passwordInput: function (e) {
    this.data.password = e.detail.value;
  },
  //登陆
  login: function () {
    var that = this;
    if (that.data.account == "test" && that.data.password == "test") {
      wx.showToast({
        title: '成功',
        icon: 'success',
        duration: 2000
      })
      wx.navigateTo({
        url: '/pages/location/location'
      }) 
    } 
    if (that.data.account == "admin" && that.data.password == "admin") {
      wx.showToast({
        title: '成功',
        icon: 'success',
        duration: 2000
      })
      wx.navigateTo({
        url: '/pages/admin/admin'
      })
    } 
    else {
      wx.showToast({
        title: '仅模拟同学可用',
        icon: 'loading',
        duration: 2000
      })
    }
  },
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
  
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