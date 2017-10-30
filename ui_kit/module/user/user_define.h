#pragma once

#include "kit_define.h"

namespace nim_comp
{
	/** @enum NIMFriendFlag 好友类型  */
	enum NIMFriendFlag
	{
		kNIMFriendFlagNotFriend = 0, /**< 陌生人 */
		kNIMFriendFlagNormal = 1, /**< 普通好友 */
	};

	/** @enum NIMFriendSource 好友来源 */
	enum NIMFriendSource
	{
		kNIMFriendSourceDefault = 0, /**< 默认 */
	};

	/** @enum NIMVerifyType 好友验证方式 */
	enum NIMVerifyType
	{
		kNIMVerifyTypeAdd = 1, /**< 直接加好友 */
		kNIMVerifyTypeAsk = 2, /**< 请求加好友 */
		kNIMVerifyTypeAgree = 3, /**< 同意 */
		kNIMVerifyTypeReject = 4, /**< 拒绝 */
	};

	/** @enum NIMFriendChangeType 好友数据变化类型 */
	enum NIMFriendChangeType
	{
		kNIMFriendChangeTypeRequest = 1, /**< 加好友/处理好友请求 */
		kNIMFriendChangeTypeDel = 2, /**< 删除好友 */
		kNIMFriendChangeTypeUpdate = 3, /**< 更新好友 */
		kNIMFriendChangeTypeSyncList = 5, /**< 好友列表同步与更新 */
	};

	/** @enum 云信用户名片数据标记Key,用以标记对应数据的有效性 */
	enum UserNameCardValueKey
	{
		kUserNameCardKeyNone = 0,			/**< 无内容 */
		kUserNameCardKeyName = 1,			/**< 昵称 */
		kUserNameCardKeyIconUrl = 1 << 1,			/**< 头像下载地址 */
		kUserNameCardKeyGender = 1 << 2,		/**< 性别 */
		kUserNameCardKeyBirthday = 1 << 3,		/**< 生日 */
		kUserNameCardKeyMobile = 1 << 4,		/**< 电话 */
		kUserNameCardKeyEmail = 1 << 5,		/**< 电子邮件 */
		kUserNameCardKeySignature = 1 << 6,		/**< 签名 */
		kUserNameCardKeyEx = 1 << 7,		/**< 扩展 */
		kUserNameCardKeyAll = (1 << 8) - 1		/**< 全部内容都有 */
	};


	struct UserNameCard
	{
		/** 构造函数，推荐使用 */
		UserNameCard(const std::string& accid) : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0)
		{
			accid_ = accid;
		}

		/** 构造函数 */
		UserNameCard() : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0) {}

	public:
		/** 设置用户ID */
		void SetAccId(const std::string& id)
		{
			accid_ = id;
		}

		/** 获得用户ID */
		std::string GetAccId() const
		{
			return accid_;
		}

		/** 设置用户名字 */
		void SetName(const std::string& name)
		{
			nickname_ = name;
			value_available_flag_ |= kUserNameCardKeyName;
		}

		/** 获得用户名字 */
		std::string GetName() const
		{
			return nickname_;
		}

		/** 设置用户头像下载地址 */
		void SetIconUrl(const std::string& url)
		{
			icon_url_ = url;
			value_available_flag_ |= kUserNameCardKeyIconUrl;
		}

		/** 获得用户头像下载地址 */
		std::string GetIconUrl() const
		{
			return icon_url_;
		}

		/** 设置用户签名 */
		void SetSignature(const std::string& sign)
		{
			signature_ = sign;
			value_available_flag_ |= kUserNameCardKeySignature;
		}

		/** 获得用户签名 */
		std::string GetSignature() const
		{
			return signature_;
		}

		/** 设置用户性别 */
		void SetGender(int gender)
		{
			gender_ = gender;
			value_available_flag_ |= kUserNameCardKeyGender;
		}

		/** 获得用户性别 */
		int GetGender() const
		{
			return gender_;
		}

		/** 设置用户邮箱 */
		void SetEmail(const std::string& email)
		{
			email_ = email;
			value_available_flag_ |= kUserNameCardKeyEmail;
		}

		/** 获得用户邮箱 */
		std::string GetEmail() const
		{
			return email_;
		}

		/** 设置用户生日 */
		void SetBirth(const std::string& birth)
		{
			birth_ = birth;
			value_available_flag_ |= kUserNameCardKeyBirthday;
		}

		/** 获得用户生日 */
		std::string GetBirth() const
		{
			return birth_;
		}

		/** 设置用户电话 */
		void SetMobile(const std::string& mobile)
		{
			mobile_ = mobile;
			value_available_flag_ |= kUserNameCardKeyMobile;
		}

		/** 获得用户电话 */
		std::string GetMobile() const
		{
			return mobile_;
		}

		/** 设置用户扩展数据 */
		void SetExpand(const Json::Value& expand)
		{
			expand_ = expand;
			value_available_flag_ |= kUserNameCardKeyEx;
		}

		/** 获得用户扩展数据 */
		Json::Value GetExpand() const
		{
			return expand_;
		}

		/** 设置用户档案创建时间戳(毫秒) */
		void SetCreateTimetag(int64_t timetag)
		{
			create_timetag_ = timetag;
		}

		/** 获得用户档案创建时间戳(毫秒) */
		int64_t GetCreateTimetag() const
		{
			return create_timetag_;
		}

		/** 设置用户档案更新时间戳(毫秒) */
		void SetUpdateTimetag(int64_t timetag)
		{
			update_timetag_ = timetag;
		}

		/** 获得用户档案更新时间戳(毫秒) */
		int64_t GetUpdateTimetag() const
		{
			return update_timetag_;
		}

		/** @fn void Update(const UserNameCard& namecard)
		* @brief 更新用户名片
		* @param[in] namecard 新的用户名片
		* @return void
		*/
		void Update(const UserNameCard& namecard)
		{
			assert(namecard.GetAccId() == accid_);
			if (namecard.GetAccId() != accid_)
				return;

			if (namecard.ExistValue(kUserNameCardKeyName))
				SetName(namecard.GetName());
			if (namecard.ExistValue(kUserNameCardKeyIconUrl))
				SetIconUrl(namecard.GetIconUrl());
			if (namecard.ExistValue(kUserNameCardKeySignature))
				SetSignature(namecard.GetSignature());
			if (namecard.ExistValue(kUserNameCardKeyGender))
				SetGender(namecard.GetGender());
			if (namecard.ExistValue(kUserNameCardKeyEmail))
				SetEmail(namecard.GetEmail());
			if (namecard.ExistValue(kUserNameCardKeyBirthday))
				SetBirth(namecard.GetBirth());
			if (namecard.ExistValue(kUserNameCardKeyMobile))
				SetMobile(namecard.GetMobile());
			if (namecard.ExistValue(kUserNameCardKeyEx))
				SetExpand(namecard.GetExpand());
			if (namecard.GetCreateTimetag() > 0)
				create_timetag_ = namecard.GetCreateTimetag();
			if (namecard.GetUpdateTimetag() > 0)
				update_timetag_ = namecard.GetUpdateTimetag();
		}

		/** @fn bool ExistValue(UserNameCardValueKey value_key) const
		* @brief 用户名片数据标记Key对应的数据是否有效（存在，非初始值状态）
		* @param[in] value_key 用户名片数据标记Key
		* @return bool 有效性
		*/
		bool ExistValue(UserNameCardValueKey value_key) const
		{
			return (value_available_flag_ & value_key) != 0;
		}

		/** @fn std::string ToJsonString() const
		* @brief 组装Json Value字符串
		* @return string Json Value字符串
		*/
		std::string ToJsonString() const
		{
#if 0
			Json::Value values;

			values[kNIMNameCardKeyAccid] = GetAccId();
			if (ExistValue(kUserNameCardKeyName))
				values[kNIMNameCardKeyName] = GetName();
			if (ExistValue(kUserNameCardKeyIconUrl))
				values[kNIMNameCardKeyIcon] = GetIconUrl();
			if (ExistValue(kUserNameCardKeySignature))
				values[kNIMNameCardKeySign] = GetSignature();
			if (ExistValue(kUserNameCardKeyGender))
				values[kNIMNameCardKeyGender] = GetGender();
			if (ExistValue(kUserNameCardKeyEmail))
				values[kNIMNameCardKeyEmail] = GetEmail();
			if (ExistValue(kUserNameCardKeyBirthday))
				values[kNIMNameCardKeyBirth] = GetBirth();
			if (ExistValue(kUserNameCardKeyMobile))
				values[kNIMNameCardKeyMobile] = GetMobile();
			if (ExistValue(kUserNameCardKeyEx))
				values[kNIMNameCardKeyEx] = GetJsonStringWithNoStyled(GetExpand());

			return GetJsonStringWithNoStyled(values);
#else
			return "";
#endif
		}

	private:
		std::string		accid_;				/**< 用户ID */
		std::string		nickname_;			/**< 用户昵称 */
		std::string		icon_url_;			/**< 用户头像下载地址 */
		std::string		signature_;			/**< 用户签名 */
		int				gender_;			/**< 用户性别 */
		std::string		email_;				/**< 用户邮箱 */
		std::string		birth_;				/**< 用户生日 */
		std::string		mobile_;			/**< 用户电话 */
		Json::Value		expand_;			/**< 用户扩展数据 */
		int64_t			create_timetag_;	/**< 用户档案创建时间戳(毫秒) */
		int64_t			update_timetag_;	/**< 用户档案更新时间戳(毫秒) */

		unsigned int	value_available_flag_;
	};

	//好友变化类型
	enum FriendChangeType
	{
		kChangeTypeAdd,
		kChangeTypeDelete
	};

	//用户性别
	enum UserGender
	{
		UG_UNKNOWN,
		UG_MALE,
		UG_FEMALE
	};
}
