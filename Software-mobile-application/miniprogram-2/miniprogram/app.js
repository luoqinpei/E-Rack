// app.js
App({
  onLaunch: function () {
    if (!wx.cloud) {
      console.error('请使用 2.2.3 或以上的基础库以使用云能力');
    } else {
      console.log("OK Cloud");
      wx.cloud.init({
        env: 'erackpku-2gfdx8lja5f1d4eb',
        traceUser: true,
      });
    }
    this.globalData = {};
  }
});
