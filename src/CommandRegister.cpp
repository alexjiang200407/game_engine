#include "CommandRegister.h"
#include <imgui.h>
#include <iostream>

namespace ImGui
{
	IMGUI_API bool
	InputText(
		const char*            label,
		std::string*           str,
		ImGuiInputTextFlags    flags     = ImGuiInputTextFlags_EnterReturnsTrue,
		ImGuiInputTextCallback callback  = NULL,
		void*                  user_data = NULL);

	CommandRegister::CommandRegister() : root(std::make_unique<Node>()) {}
	
	CommandRegister&
	CommandRegister::Instance() noexcept
	{
		static CommandRegister inst;
		return inst;
	}

	CommandRegister::Node*
	CommandRegister::FindNodeCreate(const std::string& key)
	{
		if (!root)
			root = std::make_unique<Node>();
		Node* cur = root.get();
		for (char c : key)
		{
			auto it = cur->children.find(c);
			if (it == cur->children.end())
			{
				auto  inserted = std::make_unique<Node>();
				Node* ptr      = inserted.get();
				cur->children.emplace(c, std::move(inserted));
				cur = ptr;
			}
			else
			{
				cur = it->second.get();
			}
		}
		return cur;
	}

	CommandRegister::Node*
	CommandRegister::FindNode(const std::string& key) const
	{
		if (!root)
			return nullptr;
		Node* cur = root.get();
		for (char c : key)
		{
			auto it = cur->children.find(c);
			if (it == cur->children.end())
				return nullptr;
			cur = it->second.get();
		}
		return cur;
	}

	void
	CommandRegister::PruneEmptyNodes(const std::string& key) noexcept
	{
		if (!root)
			return;
		std::vector<Node*> path;
		Node*              cur = root.get();
		path.push_back(cur);
		for (char c : key)
		{
			auto it = cur->children.find(c);
			if (it == cur->children.end())
				return;
			cur = it->second.get();
			path.push_back(cur);
		}

		Node* parent = nullptr;
		for (int i = (int)path.size() - 1; i > 0; --i)
		{
			Node* node = path[i];
			if (node->hasHandler || !node->children.empty())
				break;
			parent = path[i - 1];
			char keyChar = key[i - 1];
			parent->children.erase(keyChar);
		}
	}

    void
	CommandRegister::Process()
	{
		if (!pendingCmd)
			return;
		try
		{
			pendingCmd();
			lastError.clear();
		}
		catch (const std::exception& e)
		{
			lastError = std::string("Command threw: ") + e.what();
		}
		catch (...)
		{
			lastError = "Command threw unknown exception";
		}
		pendingCmd = nullptr;
		buf.clear();
	}


	void
	CommandRegister::Execute()
	{
		std::istringstream iss(buf);

		std::string name;
		if (!(iss >> name))
			return;

		std::shared_lock lock(mutex);
		Node*            node = FindNode(name);
		
		if (!node || !node->hasHandler)
		{
			lastError = "Unknown command: " + name;
			return;
		}

		std::string remaining;
		std::getline(iss, remaining, '\0');

        pendingCmd = [node, remaining]() {
			std::istringstream iss{ remaining };
			node->handler(iss);
		};

		lastError.clear();

	}

	void
	CommandRegister::Unregister(const std::string& cmdName) noexcept
	{
		std::unique_lock lock(mutex);
		Node*            node = FindNode(cmdName);
		if (!node)
			return;
		node->handler    = nullptr;
		node->hasHandler = false;
		PruneEmptyNodes(cmdName);
	}


	void
	CommandRegister::DrawControlWindow() noexcept
	{
		if (ImGui::InputText("Command", &buf, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Execute();
		}
		
		if (!lastError.empty())
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", lastError.c_str());
		}

	}
}

