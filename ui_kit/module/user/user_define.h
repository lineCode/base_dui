#pragma once

#include "kit_define.h"

namespace nim_comp
{
	/** @enum NIMFriendFlag ��������  */
	enum NIMFriendFlag
	{
		kNIMFriendFlagNotFriend = 0, /**< İ���� */
		kNIMFriendFlagNormal = 1, /**< ��ͨ���� */
	};

	/** @enum NIMFriendSource ������Դ */
	enum NIMFriendSource
	{
		kNIMFriendSourceDefault = 0, /**< Ĭ�� */
	};

	/** @enum NIMVerifyType ������֤��ʽ */
	enum NIMVerifyType
	{
		kNIMVerifyTypeAdd = 1, /**< ֱ�ӼӺ��� */
		kNIMVerifyTypeAsk = 2, /**< ����Ӻ��� */
		kNIMVerifyTypeAgree = 3, /**< ͬ�� */
		kNIMVerifyTypeReject = 4, /**< �ܾ� */
	};

	/** @enum NIMFriendChangeType �������ݱ仯���� */
	enum NIMFriendChangeType
	{
		kNIMFriendChangeTypeRequest = 1, /**< �Ӻ���/����������� */
		kNIMFriendChangeTypeDel = 2, /**< ɾ������ */
		kNIMFriendChangeTypeUpdate = 3, /**< ���º��� */
		kNIMFriendChangeTypeSyncList = 5, /**< �����б�ͬ������� */
	};

	/** @enum �����û���Ƭ���ݱ��Key,���Ա�Ƕ�Ӧ���ݵ���Ч�� */
	enum UserNameCardValueKey
	{
		kUserNameCardKeyNone = 0,			/**< ������ */
		kUserNameCardKeyName = 1,			/**< �ǳ� */
		kUserNameCardKeyIconUrl = 1 << 1,			/**< ͷ�����ص�ַ */
		kUserNameCardKeyGender = 1 << 2,		/**< �Ա� */
		kUserNameCardKeyBirthday = 1 << 3,		/**< ���� */
		kUserNameCardKeyMobile = 1 << 4,		/**< �绰 */
		kUserNameCardKeyEmail = 1 << 5,		/**< �����ʼ� */
		kUserNameCardKeySignature = 1 << 6,		/**< ǩ�� */
		kUserNameCardKeyEx = 1 << 7,		/**< ��չ */
		kUserNameCardKeyAll = (1 << 8) - 1		/**< ȫ�����ݶ��� */
	};


	struct UserNameCard
	{
		/** ���캯�����Ƽ�ʹ�� */
		UserNameCard(const std::string& accid) : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0)
		{
			accid_ = accid;
		}

		/** ���캯�� */
		UserNameCard() : value_available_flag_(0), create_timetag_(0), update_timetag_(0), gender_(0) {}

	public:
		/** �����û�ID */
		void SetAccId(const std::string& id)
		{
			accid_ = id;
		}

		/** ����û�ID */
		std::string GetAccId() const
		{
			return accid_;
		}

		/** �����û����� */
		void SetName(const std::string& name)
		{
			nickname_ = name;
			value_available_flag_ |= kUserNameCardKeyName;
		}

		/** ����û����� */
		std::string GetName() const
		{
			return nickname_;
		}

		/** �����û�ͷ�����ص�ַ */
		void SetIconUrl(const std::string& url)
		{
			icon_url_ = url;
			value_available_flag_ |= kUserNameCardKeyIconUrl;
		}

		/** ����û�ͷ�����ص�ַ */
		std::string GetIconUrl() const
		{
			return icon_url_;
		}

		/** �����û�ǩ�� */
		void SetSignature(const std::string& sign)
		{
			signature_ = sign;
			value_available_flag_ |= kUserNameCardKeySignature;
		}

		/** ����û�ǩ�� */
		std::string GetSignature() const
		{
			return signature_;
		}

		/** �����û��Ա� */
		void SetGender(int gender)
		{
			gender_ = gender;
			value_available_flag_ |= kUserNameCardKeyGender;
		}

		/** ����û��Ա� */
		int GetGender() const
		{
			return gender_;
		}

		/** �����û����� */
		void SetEmail(const std::string& email)
		{
			email_ = email;
			value_available_flag_ |= kUserNameCardKeyEmail;
		}

		/** ����û����� */
		std::string GetEmail() const
		{
			return email_;
		}

		/** �����û����� */
		void SetBirth(const std::string& birth)
		{
			birth_ = birth;
			value_available_flag_ |= kUserNameCardKeyBirthday;
		}

		/** ����û����� */
		std::string GetBirth() const
		{
			return birth_;
		}

		/** �����û��绰 */
		void SetMobile(const std::string& mobile)
		{
			mobile_ = mobile;
			value_available_flag_ |= kUserNameCardKeyMobile;
		}

		/** ����û��绰 */
		std::string GetMobile() const
		{
			return mobile_;
		}

		/** �����û���չ���� */
		void SetExpand(const Json::Value& expand)
		{
			expand_ = expand;
			value_available_flag_ |= kUserNameCardKeyEx;
		}

		/** ����û���չ���� */
		Json::Value GetExpand() const
		{
			return expand_;
		}

		/** �����û���������ʱ���(����) */
		void SetCreateTimetag(int64_t timetag)
		{
			create_timetag_ = timetag;
		}

		/** ����û���������ʱ���(����) */
		int64_t GetCreateTimetag() const
		{
			return create_timetag_;
		}

		/** �����û���������ʱ���(����) */
		void SetUpdateTimetag(int64_t timetag)
		{
			update_timetag_ = timetag;
		}

		/** ����û���������ʱ���(����) */
		int64_t GetUpdateTimetag() const
		{
			return update_timetag_;
		}

		/** @fn void Update(const UserNameCard& namecard)
		* @brief �����û���Ƭ
		* @param[in] namecard �µ��û���Ƭ
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
		* @brief �û���Ƭ���ݱ��Key��Ӧ�������Ƿ���Ч�����ڣ��ǳ�ʼֵ״̬��
		* @param[in] value_key �û���Ƭ���ݱ��Key
		* @return bool ��Ч��
		*/
		bool ExistValue(UserNameCardValueKey value_key) const
		{
			return (value_available_flag_ & value_key) != 0;
		}

		/** @fn std::string ToJsonString() const
		* @brief ��װJson Value�ַ���
		* @return string Json Value�ַ���
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
		std::string		accid_;				/**< �û�ID */
		std::string		nickname_;			/**< �û��ǳ� */
		std::string		icon_url_;			/**< �û�ͷ�����ص�ַ */
		std::string		signature_;			/**< �û�ǩ�� */
		int				gender_;			/**< �û��Ա� */
		std::string		email_;				/**< �û����� */
		std::string		birth_;				/**< �û����� */
		std::string		mobile_;			/**< �û��绰 */
		Json::Value		expand_;			/**< �û���չ���� */
		int64_t			create_timetag_;	/**< �û���������ʱ���(����) */
		int64_t			update_timetag_;	/**< �û���������ʱ���(����) */

		unsigned int	value_available_flag_;
	};

	//���ѱ仯����
	enum FriendChangeType
	{
		kChangeTypeAdd,
		kChangeTypeDelete
	};

	//�û��Ա�
	enum UserGender
	{
		UG_UNKNOWN,
		UG_MALE,
		UG_FEMALE
	};
}
