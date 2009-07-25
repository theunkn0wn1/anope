/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSDrop : public Command
{
 public:
	CommandCSDrop() : Command("DROP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_FORBIDDEN);
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(s_ChanServ, u, CHAN_DROP_DISABLED); // XXX: READ_ONLY_MODE?
			return MOD_CONT;
		}

		ci = cs_findchan(chan);

		if ((ci->flags & CI_FORBIDDEN) && !u->nc->HasCommand("chanserv/drop"))
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if ((ci->flags & CI_SUSPENDED) && !u->nc->HasCommand("chanserv/drop"))
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if ((ci->flags & CI_SECUREFOUNDER ? !is_real_founder(u, ci) : !is_founder(u, ci)) && !u->nc->HasCommand("chanserv/drop"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		int level = get_access(u, ci);

		if (ci->c)
		{
			if (ircd->regmode)
			{
				ci->c->mode &= ~ircd->regmode;
				ircdproto->SendMode(whosends(ci), ci->name, "-r");
			}
		}

		if (ircd->chansqline && (ci->flags & CI_FORBIDDEN))
		{
			ircdproto->SendSQLineDel(ci->name);
		}

		alog("%s: Channel %s dropped by %s!%s@%s (founder: %s)",
			 s_ChanServ, ci->name, u->nick, u->GetIdent().c_str(),
			 u->host, (ci->founder ? ci->founder->display : "(none)"));

		delchan(ci);

		/* We must make sure that the Services admin has not normally the right to
		 * drop the channel before issuing the wallops.
		 */
		if (WallDrop) {
			if ((level < ACCESS_FOUNDER) || (!is_real_founder(u, ci) && ci->flags & CI_SECUREFOUNDER))
				ircdproto->SendGlobops(s_ChanServ, "\2%s\2 used DROP on channel \2%s\2", u->nick, chan);
		}

		notice_lang(s_ChanServ, u, CHAN_DROPPED, chan);

		FOREACH_MOD(I_OnChanDrop, OnChanDrop(chan));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_DROP);
		else
			notice_help(s_ChanServ, u, CHAN_HELP_DROP);

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DROP", CHAN_DROP_SYNTAX);
	}
};

class CSDrop : public Module
{
 public:
	CSDrop(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSDrop(), MOD_UNIQUE);
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DROP);
	}
};

MODULE_INIT("cs_drop", CSDrop)
