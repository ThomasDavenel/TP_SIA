#pragma once

#include <stdext/hash.h>
#include <unordered_map>
#include <deque>

namespace stdext
{
	class message_handler
	{
	public:
		
		template <typename MessageType> friend class receiver_queue;

	private:
		/// <summary>
		/// Base class for message receivers
		/// </summary>
		class receiver_base
		{
		public:
			/// <summary>
			/// Finalizes an instance of the <see cref="receiver_base"/> class.
			/// </summary>
			virtual ~receiver_base()
			{}
		};

		template <typename MessageType>
		class template_receiver_base : public receiver_base
		{
		protected:
			message_handler * m_messageHandler;

			template_receiver_base(message_handler * messageHandler)
				: m_messageHandler(messageHandler)
			{}

		public:
			/// <summary>
			/// Posts the specified .
			/// </summary>
			/// <param name="">The .</param>
			virtual void post(const MessageType &) = 0;

			/// <summary>
			/// Finalizes an instance of the <see cref="template_receiver_base"/> class.
			/// </summary>
			virtual ~template_receiver_base()
			{
				m_messageHandler->dereference(this);
			}
		};

	public:
		/// <summary>
		/// A receiver for messages of type MessageType.
		/// </summary>
		template <typename MessageType>
		class receiver_queue : public template_receiver_base<MessageType>
		{
		protected:
			std::deque<MessageType> m_messages;


			friend class message_handler;

			/// <summary>
			/// Initializes a new instance of the <see cref="receiver"/> class.
			/// </summary>
			/// <param name="messageHandler">The message handler.</param>
			receiver_queue(message_handler * messageHandler)
				: template_receiver_base(messageHandler)
			{}

			receiver_queue(const receiver_queue &) = delete;
			receiver_queue & operator = (const receiver_queue &) = delete;

		public:
			/// <summary>
			/// Returns true if there is no more messages in the receiver
			/// </summary>
			void empty() const { return m_messages.size() == 0; }

			/// <summary>
			/// Extracts this instance.
			/// </summary>
			/// <returns></returns>
			MessageType extract() const
			{
				MessageType tmp(std::move(m_messages.front()));
				m_messages.pop_front();
				return std::move(tmp);
			}

			/// <summary>
			/// Posts the specified message.
			/// </summary>
			/// <param name="message">The message.</param>
			virtual void post(const MessageType & message) override
			{
				m_messages.push_back(message);
			}

			virtual ~receiver_queue()
			{}
		};

		/// <summary>
		/// A receiver immediately forwarding the message to a callback.
		/// </summary>
		template <typename MessageType>
		class receiver_callback : public template_receiver_base<MessageType>
		{
		protected:
			using template_receiver_base::template_receiver_base;
		
		public:
			using template_receiver_base::post;

			virtual ~receiver_callback() {}
		};

	private:
		/// <summary>
		/// A receiver immediately forwarding the message to a callback (for internal use) 
		/// </summary>
		template <typename MessageType, typename Callback>
		class template_receiver_callback : public receiver_callback<MessageType>
		{
		protected:
			friend class message_handler;

			Callback m_callback;
			
			template_receiver_callback(message_handler * messageHandler, const Callback & callback)
				: receiver_callback(messageHandler), m_callback(callback)
			{
				static_assert(std::is_invocable<Callback, MessageType>::value, "Invalid callback provided. Type should have method operator()(MessageType)");
			}

		public:
			virtual void post(const MessageType & msg) override
			{
				m_callback(msg);
			}

			virtual ~template_receiver_callback()
			{}
		};

	private:
		//std::unordered_map<std::reference_wrapper<const type_info>, std::vector<receiver_base*>> m_callbacks;
		std::unordered_map<std::reference_wrapper<const type_info>, std::unordered_map<void*, std::vector<receiver_base*>>> m_callbacks;

		/// <summary>
		/// Dereferences an instance of receiver.
		/// </summary>
		/// <param name="receiver">The receiver.</param>
		template <typename MessageType>
		void dereference(template_receiver_base<MessageType> * receiver)
		{
			std::unordered_map<void*, std::vector<receiver_base*>> & current = m_callbacks[typeid(MessageType)];
			for (auto it = current.begin(), end = current.end(); it != end; ++it)
			{
				std::vector<receiver_base*> & tmp = it->second;
				auto found = std::find(tmp.begin(), tmp.end(), receiver);
				if (found != tmp.end())
				{
					(*found) = tmp.back();
					tmp.pop_back();
				}
			}
		}

		/// <summary>
		/// Posts the specified message.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="receiver">The receiver.</param>
		template <typename MessageType>
		void postHelper(const MessageType & message, receiver_base * receiver)
		{
			{
				receiver_queue<MessageType> * current = dynamic_cast<receiver_queue<MessageType>*>(receiver);
				if (current != nullptr)
				{
					current->post(message);
					return;
				}
			}
			{
				receiver_callback<MessageType> * current = dynamic_cast<receiver_callback<MessageType> *>(receiver);
				if (current != nullptr)
				{
					current->post(message);
					return;
				}
			}
		}

	public:
		/// <summary>
		/// Broadcasts the specified message to all receivers of type receiver<MessageType>
		/// </summary>
		/// <param name="message">The message.</param>
		template <typename MessageType>
		void broadcast(const MessageType & message)
		{
			const std::unordered_map<void*, std::vector<receiver_base*>> & selected = m_callbacks[typeid(MessageType)];
			for (auto selectedIt = selected.begin(), end = selected.end(); selectedIt != end; ++selectedIt)
			{
				const std::vector<receiver_base*> & receivers = selectedIt->second;
				for (auto it = receivers.begin(), end = receivers.end(); it != end; ++it)
				{
					postHelper(message, *it);
				}
			}
		}

		/// <summary>
		/// Posts the specified message to the specified target.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="target">The target.</param>
		template <typename MessageType, typename TargetType>
		void post(const MessageType & message, TargetType * target)
		{
			const std::unordered_map<void*, std::vector<receiver_base*>> & selected = m_callbacks[typeid(MessageType)];
			auto it = selected.find(target);
			if (it == selected.end()) { return; }
			for (auto receiverIt = it->second.begin(), end = it->second.end(); receiverIt != end; ++receiverIt)
			{
				postHelper(message, *receiverIt);
			}
			if (target != spyTarget()) { post(message, spyTarget()); }
		}

		/// <summary>
		/// Posts the specified message to the provided group of targets.
		/// </summary>
		/// <param name="message">The message.</param>
		/// <param name="targets">The targets.</param>
		template <typename MessageType, typename TargetType>
		void post(const MessageType & message, const std::vector<TargetType*> & targets)
		{
			for (auto it = targets.begin(), end = targets.end(); it != end; ++it)
			{
				post(message, *it);
			}
		}

		/// <summary>
		/// Creates a new queue receiver.
		/// </summary>
		/// <returns></returns>
		template <typename MessageType, typename TargetType>
		receiver_queue<MessageType> * createReceiver(TargetType * receiverIdentifier)
		{
			receiver_queue<MessageType> * created = new receiver_queue<MessageType>(this);
			m_callbacks[std::cref(typeid(MessageType))][receiverIdentifier].push_back(created);
			return created;
		}

		/// <summary>
		/// Creates a new callback receiver.
		/// </summary>
		/// <param name="callback">The callback.</param>
		/// <returns></returns>
		template <typename MessageType, typename Callback, typename TargetType>
		receiver_callback<MessageType> * createReceiver(const Callback & callback, TargetType * receiverIdentifier)
		{
			receiver_callback<MessageType> * created = new template_receiver_callback<MessageType, Callback>(this, callback);
			m_callbacks[std::cref(typeid(MessageType))][receiverIdentifier].push_back(created);
			return created;
		}

		/// <summary>
		/// A specific target that is a spy. A spy receives all messages exchanged between all entities.
		/// </summary>
		/// <returns></returns>
		constexpr void * spyTarget() const { return nullptr; }
	};
}