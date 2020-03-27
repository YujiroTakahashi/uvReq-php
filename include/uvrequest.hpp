#pragma once

#include <chrono>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <uv.h>
#include <curl/curl.h>

namespace croco {

/**
 * uvRequest
 *
 * @package     UV-Request
 * @author      Yujiro Takahashi <yujiro@cro-co.co.jp>
 */
class uvRequest {
public:
    typedef struct _Node {
        std::string url;
        std::string reply;
    } Node;

    typedef struct _Work {
        std::string url;
        std::string reply;
        bool requestOK;
        int tryNumber;
        uvRequest* self;
    } Work;

private:
    uv_loop_t *_loop;
    std::vector<Node> _nodes;
    std::vector<std::string> _errs;
    long _timeout;
    long _sleepTime;
    std::string _ua;

public:
    void setTimeout(long timeout);
    void setUserAgent(std::string ua);
    void setSleepTime(long sleepTime);
    long getSleepTime();
    void setWorker();
    void await();
    void queueWork(std::string url, int tryNumber);
    std::vector<Node> getReply();
    std::vector<std::string> getErrors();

private:
    static void _work_cb(uv_work_t* req);
    static void _after_work_cb(uv_work_t* req, int status);
    static std::size_t _receive_cb(char* ptr, std::size_t size, std::size_t nmemb, void* data);
    std::string _request(std::string url);
    void _addNode(std::string url, std::string reply);
    void _addError(std::string url);

}; // class uvRequest

/**
 * タイムアウトの設定
 *
 * @access public
 * @param  long timeout
 * @return void
 */
inline void uvRequest::setTimeout(long timeout)
{
    _timeout = timeout;
}

/**
 * ユーザーエージェントの設定
 *
 * @access public
 * @param  std::string ua
 * @return void
 */
inline void uvRequest::setUserAgent(std::string ua)
{
    _ua.assign(ua);
}

/**
 * スリープ時間（ミリ秒）の設定
 *
 * @access public
 * @param  long sleepTime
 * @return void
 */
inline void uvRequest::setSleepTime(long sleepTime)
{
    _sleepTime = sleepTime;
}

/**
 * スリープ時間（ミリ秒）の取得
 *
 * @access public
 * @return long
 */
inline long uvRequest::getSleepTime()
{
    return _sleepTime;
}

/**
 * ループイベントの確保
 *
 * @access public
 * @return void
 */
inline void uvRequest::setWorker()
{
    _loop = uv_default_loop();
    _nodes.clear();
    _errs.clear();
}

/**
 * ワーカーの起動と実行待ち
 *
 * @access public
 * @return void
 */
inline void uvRequest::await()
{
    uv_run(_loop, UV_RUN_DEFAULT);
}

/**
 * ワーカーの登録
 *
 * @access public
 * @param  std::string sentence
 * @return void
 */
inline void uvRequest::queueWork(std::string url, int tryNumber)
{
    uv_work_t *req = new uv_work_t();
    Work *data = new Work();

    data->url = url;
    data->tryNumber = tryNumber;
    data->self = this;
    req->data = data;

    uv_queue_work(_loop, req, &_work_cb, &_after_work_cb);
}

/**
 * 取得した全てのレスポンスを取得
 *
 * @access public
 * @return uvRequest::Node
 */
inline std::vector<uvRequest::Node> uvRequest::getReply()
{
    return _nodes;
}

/**
 * エラーURLを取得
 *
 * @access public
 * @return std::vector<std::string>
 */
inline std::vector<std::string> uvRequest::getErrors()
{
    return _errs;
}

/**
 * ワーカーコールバック
 *
 * @access private
 * @param  uv_work_t* req
 * @return void
 */
inline void uvRequest::_work_cb(uv_work_t* req)
{
    Work* work = static_cast<Work*>(req->data);

    try {
        work->tryNumber--;
        work->reply = work->self->_request(work->url);
        work->requestOK = true;
    } catch (...) {
        work->requestOK = false;
        std::chrono::milliseconds ms(work->self->getSleepTime());
        std::this_thread::sleep_for(ms);
    } // try
}

/**
 * ワーカーステータスコールバック
 *
 * @access private
 * @param  uv_work_t* req
 * @param  int status
 * @return void
 */
inline void uvRequest::_after_work_cb(uv_work_t* req, int status)
{
    Work* work = static_cast<Work*>(req->data);

    if (work->requestOK) {
        work->self->_addNode(work->url, work->reply);
    } else {
        if (0 < work->tryNumber) {
            work->self->queueWork(work->url, work->tryNumber);
        } else {
            work->self->_addError(work->url);
        }
    } // if (work->requestOK)

    delete work;
    delete req;
}

/**
 * レシーブコールバック
 *
 * @access private
 * @param  char* ptr
 * @param  std::size_t size
 * @param  std::size_t nmemb
 * @param  void* data
 * @return void
 */
inline std::size_t uvRequest::_receive_cb(char* ptr, std::size_t size, std::size_t nmemb, void* data)
{
    std::string *recvData = static_cast<std::string*>(data);
    const std::size_t dataLength = (size * nmemb);
    recvData->append(ptr, dataLength);

    return dataLength;
}

/**
 * URLをリクエスト
 *
 * @access public
 * @param  std::string url
 * @return void
 */
inline std::string uvRequest::_request(std::string url)
{
    std::string reply("");

    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_easy_cleanup(curl);
        throw std::logic_error("curl_easy_init() failed.");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, _ua.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_receive_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeout);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error("not ok.");
    }

    if (0 >= reply.length()) {
        throw std::runtime_error("no reply.");
    }

    return reply;
}

/**
 * URLとレスポンスの保存
 *
 * @access private
 * @param  std::string url
 * @param  std::string reply
 * @return void
 */
inline void uvRequest::_addNode(std::string url, std::string reply)
{
    _nodes.push_back({url, reply});
}

/**
 * エラーだったURLの保存
 *
 * @access private
 * @param  std::string url
 * @return void
 */
inline void uvRequest::_addError(std::string url)
{
    _errs.push_back(url);
}


} // namespace croco
