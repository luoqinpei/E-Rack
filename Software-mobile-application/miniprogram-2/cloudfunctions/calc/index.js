// 云函数入口文件
const cloud = require('wx-server-sdk')
const db = wx.cloud.database();

cloud.init()

// 云函数入口函数
exports.main = async (event, context) => {
  var temp,humid,weight,time;
  db.collection('weight').where({
    _id: 'weight',
    _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
  }).get({
    success: function(res) {
      _weight = parseInt(res.data[0].weight),
      time = parseInt(res.data[0].time),
      vel = parseInt(res.data[0].vel)
  }
  })
  delta = (Date().getTime() - time) / 1000 / 60;
  _weight = _weight - delta * vel;
  db.collection('current_sensor').where({
    _id: '2b6383996458e95f001249976b12df2c',
    _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
  }).get({
    success: function(res) {
        temp = parseInt(res.data[0].temp),
        humid = parseInt(res.data[0].humid)
    }
  });
  p_s = 610.78*exp((temp)/(temp+238.3)*17.2604);
  p = humid * p_s;
  v = (0.002166)*p/(temp+273.16);
  console.log(delta);
  console.log(v);
  db.collection('weight').where({
    _id: 'weight',
    _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
  }).update({
    data: {
        weight : _weight,
        vel: v
    }
  })
  return {
    hour : _weight / v
  }
}