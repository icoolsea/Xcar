#ifndef LOG_
#define LOG_

#define LOG_ERROR                                                              \
    qDebug() << "[ERROR]:" << __FILE__ << __LINE__ << __FUNCTION__ << ":"

#define LOG_INFO                                                               \
    qDebug() << "[INFO]:" << __FILE__ << __LINE__ << __FUNCTION__ << ":"

#define LOG_DEBUG                                                              \
    qDebug() << "[DEBUG]:" << __FILE__ << __LINE__ << __FUNCTION__ << ":"

#endif // LOG_
