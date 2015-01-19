/***************************************************************************
 *
 * Copyright (c) 2009 FastRpc
 *
 **************************************************************************/

/**
 * @file scopelocale.h
 * @author hemingzhe(feimat@qq.com)
 * @date 2009/09/11 17:51:21
 * @brief
 *
 **/

#ifndef  __SCOPELOCALE_H_
#define  __SCOPELOCALE_H_

#include <stdlib.h>
#include <string.h>
#include <locale.h>

/**
 * @brief
 */
class ScopeLocale {
public:
    /**
     * @brief ScopeLocale�Ĺ��캯��.
     *
     * ScopeLocale�����ڿ�ʼ, ��locale�е�LC_CTYPE����Ϊָ��ֵ.
     *
     * @param locale ָ��LC_CTYPE��locale.
     */
    ScopeLocale(const char *locale) {
        saved_locale_ = strdup(setlocale(LC_CTYPE, NULL));
        setlocale(LC_CTYPE, locale);
    }

    /**
     * @brief ScopeLocale����������.
     *
     * ScopeLocale�����ڽ���, ��locale�е�LC_CTYPE�ָ�Ϊ��ʼֵ.
     */
    ~ScopeLocale() {
        setlocale(LC_CTYPE, saved_locale_);
        free(saved_locale_);
    }

private:
    char *saved_locale_;
};

class GBKAutoScopeLocale : public ScopeLocale {
public:
#ifdef WIN32
    GBKAutoScopeLocale() : ScopeLocale("chs_chn") {
    }
#else
    GBKAutoScopeLocale() : ScopeLocale("zh_CN.GBK") {
    }
#endif
};

class UTF8AutoScopeLocale : public ScopeLocale {
public:
#ifdef WIN32
    UTF8AutoScopeLocale() : ScopeLocale("chs_chn") {
    }
#else
    UTF8AutoScopeLocale() : ScopeLocale("zh_CN.utf-8") {
    }
#endif
};

#endif  //__SCOPELOCALE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
