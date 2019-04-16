const Koa = require('koa')
const app = new Koa()
const debug = require('debug')('koa-weapp-demo')
const response = require('./middlewares/response')
const bodyParser = require('koa-bodyparser')
const config = require('./config')

// 使用响应处理中间件
app.use(response)

// 解析请求体,解析原始request请求的body,即post里的表单或者json数据,把解析后的参数绑定到ctx.request.body中
app.use(bodyParser())

// 引入路由分发，weapp/xxx 请求url集中管理处理
const router = require('./routes')
app.use(router.routes())

// 启动程序，监听端口： 5757
app.listen(config.port, () => debug(`listening on port ${config.port}`))
