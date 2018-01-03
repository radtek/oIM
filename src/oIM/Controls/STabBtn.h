#ifndef __STABBTN_BY_YFGZ_20180102_HEADER__
#define __STABBTN_BY_YFGZ_20180102_HEADER__

/**
 * @class      SButton
 * @brief      按钮控件类
 * 
 * Describe    通过属性ID绑定click事件 Use id attribute to process click event
 * Usage       <button id=xx>inner text example</button>
 */
class STabBtn: public SWindow
    , public IAcceleratorTarget
    , public ITimelineHandler
{
    SOUI_CLASS_NAME(STabBtn, L"tabbtn")
public:
    /**
     * SButton::SButton
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    STabBtn();

public:
        /**
     * SButton::GetDesiredSize
     * @brief    获得期望的大小值
     * @param    LPRECT pRcContainer -- 内容窗体矩形
     *
     * Describe  根据内容窗体矩形大小，计算出适合的大小
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);

protected:
    /**
     * SButton::NeedRedrawWhenStateChange
     * @brief    状态变化需要重画
     * @return   返回值BOOL 成功--TRUE 失败--FALSE
     *
     * Describe  当按钮状态发生变化时候需要重新绘制 默认返回TRUE
     */
    virtual BOOL NeedRedrawWhenStateChange()
    {
        return TRUE;
    }
    /**
     * SButton::OnGetDlgCode
     * @brief    获得编码
     *
     * Describe  返回宏定义SC_WANTCHARS代表需要WM_CHAR消息
     */
    virtual UINT OnGetDlgCode()
    {
        return SC_WANTCHARS;
    }

    /**
     * SButton::OnAcceleratorPressed
     * @brief    加速键按下
     * @param    CAccelerator& accelerator -- 加速键相关结构体
     * @return   返回值BOOL 成功--TRUE 失败--FALSE
     *
     * Describe  处理加速键响应消息
     */
    virtual bool OnAcceleratorPressed(const CAccelerator& accelerator);

protected:
    virtual void OnStateChanged(DWORD dwOldState,DWORD dwNewState);
    void OnPaint(IRenderTarget *pRT);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnDestroy();
    void OnSize(UINT nType, CSize size);
    BOOL OnEraseBkgnd(IRenderTarget * pRT){return TRUE;}
    HRESULT OnAttrAccel(SStringW strAccel,BOOL bLoading);

protected:
    virtual void OnNextFrame();
    void StopCurAnimate();

    DWORD  m_accel;
    BOOL   m_bAnimate;    /**< 动画标志 */
    BYTE   m_byAlphaAni;  /**< 动画状态 */
public:
    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"accel",OnAttrAccel)
        ATTR_INT(L"animate", m_bAnimate, FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown) //将双击消息处理为单击
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
    SOUI_MSG_MAP_END()
};


#endif //__STABBTN_BY_YFGZ_20180102_HEADER__

