/* Declarations for command data.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "services.h"

class Module;
class BotInfo;
class Command;

typedef std::map<Anope::string, Command *, std::less<ci::string> > CommandMap;

/** The return value from commands.
 */
enum CommandReturn
{
	MOD_CONT,
	MOD_STOP
};

extern CoreExport Command *FindCommand(BotInfo *bi, const Anope::string &cmd);
extern CoreExport void mod_help_cmd(BotInfo *bi, User *u, const Anope::string &cmd);
extern CoreExport void mod_run_cmd(BotInfo *bi, User *u, const Anope::string &message, bool fantasy);
extern CoreExport void mod_run_cmd(BotInfo *bi, User *u, Command *c, const Anope::string &command, const Anope::string &message, bool fantasy);

enum CommandFlag
{
	CFLAG_ALLOW_UNREGISTERED,
	CFLAG_ALLOW_FORBIDDEN,
	CFLAG_ALLOW_SUSPENDED,
	CFLAG_ALLOW_UNREGISTEREDCHANNEL,
	CFLAG_STRIP_CHANNEL,
	CFLAG_DISABLE_FANTASY
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Flags<CommandFlag>
{
 public:
 	/* Maximum paramaters accepted by this command */
	size_t MaxParams;
	/* Minimum parameters required to use this command */
	size_t MinParams;
	/* Command name */
	Anope::string name;
	/* Permission needed to use this comand */
	Anope::string permission;

	/* Module which owns us */
	Module *module;
	/* Service this command is on */
	BotInfo *service;

	/** Create a new command.
	 * @param sname The command name
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(const Anope::string &sname, size_t min_params, size_t max_params = 0, const Anope::string &spermission = "");

	virtual ~Command();

	/** Execute this command.
	 * @param u The user executing the command.
	 */
	virtual CommandReturn Execute(User *u, const std::vector<Anope::string> &);

	/** Called when HELP is requsted for the client this command is on.
	 * @param u The user requesting help
	 */
	virtual void OnServHelp(User *u);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param u The user requesting help
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(User *u, const Anope::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param u The user executing the command.
	 * @param subcommand The subcommand the user tried to use
	 */
	virtual void OnSyntaxError(User *u, const Anope::string &subcommand);

	/** Set which command permission (e.g. chanserv/forbid) is required for this command.
	 * @param reststr The permission required to successfully execute this command
	 */
	void SetPermission(const Anope::string &reststr);

	/** Add a subcommand to this command
	 * @param c The command
	 */
	virtual bool AddSubcommand(Command *c);

	/** Delete a subcommand from this command
	 * @param c The command
	 */
	virtual bool DelSubcommand(Command *c);

	/** Find a subcommand
	 * @param name The subcommand name
	 * @return The subcommand
	 */
	virtual Command *FindSubcommand(const Anope::string &subcommand);
};

#endif // COMMANDS_H
