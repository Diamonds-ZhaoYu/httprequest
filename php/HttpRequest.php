<?PHP
// +----------------------------------------------------------------------
// |  Request: 处理 HTTP 请求相关
// +----------------------------------------------------------------------
// | Copyright (c) https://admuch.txbapp.com All rights reserved.
// +----------------------------------------------------------------------
// | Author: zhaoyu <9641354@qq.cn>
// +----------------------------------------------------------------------
// | Date: 2019/11/09 4:41 下午
// +----------------------------------------------------------------------
class HttpRequest
{

    /**
     * Instance parameters
     * @var array
     */
    protected $_params = array();


    /**
     * Instance parameters
     * @var bool
     */
    private $_is_https = false;


    /**
     * Request constructor.
     * @param array $option
     */
	private function __construct()
	{
        $this->_params['CLIENT_IP']    = $this->getIp();
        $this->_params['CLIENT_AGENT'] = $this->getAgent();
        $this->_params['REQUEST_TIME'] = isset($_SERVER['REQUEST_TIME']) ? $_SERVER['REQUEST_TIME'] : time();
        $this->_params['HTTP_REFERER'] = isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : '';
	}


    /**
     *
     * return current singleton self
     *
     * @return Request
     */
	public static function current()
	{
        static $_current;
        if ($_current === null) $_current = new self();
        return $_current;
	}

	/**
	 * 返回相关参数
	 *
	 * @param string $key
	 * @return void
	 */
	public function __get($key)
	{
        switch (true) {
            case array_key_exists($key, $this->_params):
                return $this->_params[$key];

            case array_key_exists($key, $_GET):

                return trim($_GET[$key]);

            case array_key_exists($key, $_POST):
                return trim($_POST[$key]);

            case array_key_exists($key, $_COOKIE):
                return $_COOKIE[$key];

            case array_key_exists($key, $_SERVER):
                return $_SERVER[$key];

            case array_key_exists($key, $_ENV):
                return $_ENV[$key];

            default:
                return null;
        }
	}

    /**
     * Alias to __get
     *
     * @param string $key
     * @return mixed
     */
    public function get($key)
    {
        return $this->__get($key);
    }

	/**
	 * function description
	 *
     * @param string $key
     * @param mixed $value
	 * @return void
	 */
	public function __set($key, $value)
	{
        $key = (string) $key;

        if ((null === $value) && isset($this->_params[$key])) {
            unset($this->_params[$key]);
        } elseif (null !== $value) {
            $this->_params[$key] = $value;
        }

	}

    /**
     * Alias to __set()
     *
     * @param string $key
     * @param mixed $value
     * @return object
     */
    public function set($key, $value)
    {
		$this->__set($key, $value);
        return $this;
    }


    /**
     * 是否是POST
     * @return bool
     */
	public function isPost()
	{
		return isset($_POST) && 'POST' === $_SERVER['REQUEST_METHOD'];
	}


    /**
     * 返回浏览器信息
     * @return mixed|string
     */
	public function getAgent()
	{
		return isset($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT'] : '';
	}



    /**
     * 取得访问者IP
     * @return mixed|string
     */
	public function getIp()
	{
		if (isset($_SERVER["HTTP_X_FORWARDED_FOR"]))
		{
			return $_SERVER["HTTP_X_FORWARDED_FOR"];
		}
		elseif (isset($_SERVER["HTTP_CLIENT_IP"]))
		{
			return $_SERVER["HTTP_CLIENT_IP"];
		}
		elseif(isset($_SERVER["REMOTE_ADDR"]))
		{
			return $_SERVER["REMOTE_ADDR"];
		}
		return '';
	}


    /**
     * check schema is https
     *
     * @return array|bool
     */
	public function isHttps()
	{
		if (!is_null($this->_is_https)) return $this->_is_https;
		$this->_is_https = isset($_SERVER['HTTPS']) && strtolower($_SERVER['HTTPS']) == 'on';
		return $this->_is_https;
	}
}

