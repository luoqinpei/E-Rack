//index.js
//获取应用实例
const app = getApp();
const db = wx.cloud.database();
var num = 10;
var weather_last_id = null;

Page({
  data: {
    province:'',
    city:'',
    temp:'',
    temperature:'',
    humid:'',
    humidity:'',
    rain:'',
    hour: 4,
    windspeed:'',
    weather:'',
    weather_src:'',
    time:'',
    adloc:'510703',
    rack_state: '',
    rack_state_text:'收起',
    recommend_state: '0',
    pop_index: '0',
    recommend_state_text1: '',
    recommend_state_text2: '',
    recommend_state_src: '',
    num: num,
    isShowConfirm: 1,
    abnorm: 0,
  },
  onLoad(){
    var self = this;
    db.collection('weight').where({
      _id: 'weight',
      _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
    }).get({
      success: function(res) {
        var _weight = parseFloat(res.data[0].weight);
        var time = parseInt(res.data[0].time);
        var vel = parseFloat(res.data[0].vel);
        var date = new Date;
        var delta = (date.getTime() - time) / 1000 / 60;
        var _weight;
        _weight = _weight - delta * vel/600;
        if (_weight < 0)
          _weight = 0;
        db.collection('current_sensor').where({
          _id: '2b6383996458e95f001249976b12df2c',
          _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
        }).get({
          success: function(res) {
              var temp = parseInt(res.data[0].temp);
              var humid = parseInt(res.data[0].humid);
              var rain = parseInt(res.data[0].rain);
              var p_s = 610.78*Math.exp((temp)/(temp+238.3)*17.2604);
              var p = humid * p_s;
              var v = (0.002166)*p/((temp+273.16));
              var state;
              if (rain == 1)
                state = '2';
              else
              {
                if (temp > 30 || temp < 0 || humid > 90 || self.data.abnorm == 1)
                  state = '2';
                else if (temp >= 20 && temp <=30 && humid >=20 && humid <= 60)
                  state = '0';
                else
                  state = '1'; 
              }
              self.setData({
                hour : (6*_weight / v).toFixed(2),
                recommend_state: state
              });
              db.collection('weight').where({
                _id: 'weight',
                _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
              }).update({
                data: {
                    weight : _weight,
                    vel: v,
                    time: date.getTime()
                },
              });
          }
        });
    }
    })

    db.collection('command').where({
      _id: 'rack_command',
      _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
    }).get({
      success: function(res) {
          self.setData({
            rack_state : res.data[0].command
          });
          if (self.data.rack_state == '0')
          {
            self.setData({
              rack_state_text: '收起'
            })
          }
          else
          {
            self.setData({
              rack_state_text: '晾出'
            })
          }
      }
    });
    switch(self.data.recommend_state)
    {
      case '0':
        self.setData({
          recommend_state_text1: '当前环境条件良好',
          recommend_state_text2: '建议您晾晒衣物',
          recommend_state_src: '../../images/icons/good.png'
        })
        break;
      case '1':
        self.setData({
          recommend_state_text1: '当前环境条件不佳',
          recommend_state_text2: '建议您考虑实际环境决定是否晾晒',
          recommend_state_src: '../../images/icons/soso.png'
        })
        break;
      case '2':
        self.setData({
          recommend_state_text1: '当前环境条件恶劣',
          recommend_state_text2: '建议您收回衣物',
          recommend_state_src: '../../images/icons/bad.png'
        })
        break;
      default:
        break;
    }

    db.collection('current_sensor').where({
      _id: '2b6383996458e95f001249976b12df2c',
      _openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'
    }).get({
      success: function(res) {
          self.setData({
            temp : res.data[0].temp.toFixed(2),
            humid : res.data[0].humid.toFixed(2),
            rain: res.data[0].rain
          });
      }
    });    

    // 地理位置信息API
    wx.getLocation({
      type: 'wgs84',
      success (res) {
        wx.request({
          url: 'https://restapi.amap.com/v3/geocode/regeo?parameters',
          data:{
            'key': '12089e42c92384166faecadd9d2c6a09',//填入自己申请到的Key
            'location': res.longitude+','+res.latitude,
          },
          header:{
            'content-type': 'application/json'
          },
          success:function(res){
            self.setData({
              adloc:res.data.regeocode.addressComponent.adcode,
            })
    // 天气API接口
      wx.request({
        url: 'https://restapi.amap.com/v3/weather/weatherInfo',
        data:{
          'key': '12089e42c92384166faecadd9d2c6a09',//填入自己申请到的Key
          'city': self.data.adloc,
        },
        header:{
          'content-type': 'application/json'
        },
        success:function(res){
          self.setData({
            province: res.data.lives[0].province,
            city: res.data.lives[0].city,
            temperature: res.data.lives[0].temperature,
            humidity: res.data.lives[0].humidity,
            windspeed: res.data.lives[0].windpower,
            weather: res.data.lives[0].weather,
            time: res.data.lives[0].reporttime
          })
          if (weather_last_id != null)
          {
            db.collection("weather_data").doc(weather_last_id).get({
              success: function(res) {
                if(res.data.time != self.data.time)
                db.collection("weather_data").add({
                  data: self.data,
                  success: function(res) {
                    weather_last_id = res._id;
                  }
                })
              }
            })
          }

          if(weather_last_id == null)
          db.collection("weather_data").add({
            data: self.data,
            success: function(res) {
              weather_last_id = res._id;
            }
          })
          self.weather_src = "";
          var ind;
          ind = self.data.weather.indexOf('晴');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/sunny.png"
      })}
          ind = self.data.weather.indexOf('云');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/cloudy.png"
      })}
          ind = self.data.weather.indexOf('雨');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/rainy.png"
      })}
          ind = self.data.weather.indexOf('阴');
          if (ind != -1) 
      {            self.setData({
          weather_src: "../../images/weather_icon/overcast.png"
        })};
          ind = self.data.weather.indexOf('霾');
          if (ind != -1) 
      {      
        self.setData({
          weather_src: "../../images/weather_icon/haze.png"
        })
      }
          ind = self.data.weather.indexOf('沙');
          var ind2 = self.data.weather.indexOf('尘');
          if (ind != -1 || ind2 != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/sanddust.png",
        abnorm : 1
      })}
          ind = self.data.weather.indexOf('风');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/windy.png"
      })}
          ind = self.data.weather.indexOf('雪');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/snowy.png"
      })}
          ind = self.data.weather.indexOf('雾');
          if (ind != -1) 
      {            self.setData({
        weather_src: "../../images/weather_icon/foggy.png"
      })}
        }
      })
          }
        })
        }
      })
      if (this.data.recommend_state=='2' && this.data.rack_state=='1' && this.data.pop_index!='1')
      {
        this.start();
      }
  },

  fresh: function(){
    this.setData({
      pop_index: '0'
    })
    this.onLoad();
  },

  out_rack: function()
  {
    if (this.data.rack_state == '0')
    {      
      this.setData({
        rack_state : '1'
      });
      db.collection("command").doc("rack_command").update({
        data : {
            command: '1'
        }
      })
      setTimeout(this.onLoad, 500);
    }
  },

  in_rack: function()
  {
    if (this.data.rack_state == '1')
    {      
      this.setData({
        rack_state : '0'
      });
      db.collection("command").doc("rack_command").update({
        data : {
            command: '0'
        }
      });
      setTimeout(this.onLoad, 500);
    }
  },

  confirm: function()
  {
    num = 10;
    this.in_rack();
  },

  cancel: function()
  {
    num = 10;
    this.setData({
      pop_index: '1'
    })
    this.onLoad()
  },

  start: function () {
    if(this.data.rack_state == '0' || this.data.pop_index == '1')
     return;
    this.setData({
      num: num --
    })
    this.timer()
  },

  timer: function () { 
    if (this.data.num > 0) {
      setTimeout(this.start, 1000);
    } 
    else {
      this.in_rack();
      num = 10;
    }
  },

  setValue: function (e) {
    var date = new Date;
    db.collection("weight").doc("weight").update({
      data : {
          weight: e.detail.value,
          time: date.getTime(),
      }
    })
  },

  confirmAcceptance:function(){
    var that = this
    that.setData({
      isShowConfirm: false,
    })
    this.onLoad();
  },

})