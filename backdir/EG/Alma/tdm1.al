%ALMA-based dialog manager
%by Carl Andersen and David Traum 1997-1999

%testing
test.
fif(test,conclusion(call(ah_init,1))).

%utterance creation
fif(and(new_message(ID,Asserts),
        eval_bound(\+ pos_int(call(ah_preprocess_msg(ID,Asserts),ID)), [ID,Asserts])),
conclusion(call(ah_preprocess_msg(ID,Asserts),ID))).

fif(and(done(ah_preprocess_msg(ID,Asserts),ID),
    sender(ID,parser)),
conclusion(lreq(utterance(ID),ID))).

fif(and(done(ah_preprocess_msg(ID,Asserts),ID),
    sender(ID,ps)),
conclusion(psmsg(ID))).

%really should specify which rule to check first
%fif(and(new_message(ID,Asserts),
%    and(new_message_kv(ID,Asserts1),
%    and(eval_bound(\+ pos_int(done(added_kvs(ID))),[ID]),
%        eval_bound(append(asserts,Asserts1,Asserts2),[Asserts1,Asserts2])))),
%conclusion(and(done(added_kvs(ID)),
%               df(new_message(ID,Asserts)),
%               new_message(ID,Asserts2)))).


%fif(psmsg(ID),
% conclusion(call(ah_kqml_keyvals(ID),ID))).

%reply handling
fif(and(psmsg(ID2),
    and('in-reply-to'(ID2,ID3),
	msgname(ID1,ID3))),
conclusion(psreply(ID1,ID2))).



%**********************************************
%**********************************************

%send the new york train to boston

%utterance to interpretation
fif(lreq(utterance(ID),ID), 
conclusion(compute_ireq(ID))).

fif(and(compute_ireq(ID),
    and(new_message(ID,Asserts),
    and(new_message_kv(ID,Asserts1),
        eval_bound(append(Asserts,Asserts1,Asserts2),[Asserts,Asserts1])))),
conclusion(call(ah_translate_parse(ID,Asserts2),ID))).

fif(or(done(ah_translate_parse(ID,Asserts),ID),created_msg(ID)),
conclusion(done(compute_ireq(ID)))).

%does one want to disambiguate all Objs at once or one at a time?
%no easy way to gather up predicates with bound vars
fif(done(compute_ireq(ID)),
conclusion(compute_dreq(ID))).

fif(compute_dreq(ID),
conclusion(copy_ireq_dreq(ID))).

%this is awkward - there should be a way to express this more logically
fif(and(copy_ireq_dreq(ID),
    and(eval_bound(gather_all(ireq(Assert,ID),Asserts),[ID]),
        eval_bound(copy_all(dreq,Asserts),[Asserts]))),
conclusion(done(copy_ireq_dreq(ID)))).

%this uses a hack in that it will evaluate every time a new disambiguation 
%query is translated
fif(and(or(done(copy_ireq_dreq(ID)),
           done(ah_translate_queryans(ID,_,_),_)),
    and(eval_bound(\+ pos_int(done(compute_dreq(ID))), [ID]),
    and(eval_bound(log_findall(Obj,[dreq(obj(ID,Obj),ID),dreq(lex(Obj,null),ID)],[],Objs),[ID]),
        eval_bound(Objs == [],[Objs])))),
conclusion(done(compute_dreq(ID)))).

fif(and(done(copy_ireq_dreq(ID)),
    and(ireq(obj(ID,Obj),ID),
        ireq(lex(Obj,null),ID))),
conclusion(dreq(disambiguate(ID,Obj),ID))).

%ask ps about unbound vars
%really would like a mechanism here to check
%FORALL X (obj(X) -> lex(X,Y) ^ not(null(Y)))
fif(and(dreq(disambiguate(ID,Obj),ID),
    and(ireq(type(ID,'sa-request'),ID),
    and(ireq(sem(ID,PSem),ID),
    and(ireq(path(ID,Path),ID),
    and(ireq(to(Path,To),ID),
        eval_bound(mult_gather_all([
                     ireq(_,ID),
                     dreq(psConstraint(ID,Constraint,Args),ID)],
                     Asserts),[ID])))))),
conclusion(call(ah_askquestion(ID,all,ID2,Asserts,Asserts1),ID))).
%added ID to eval_bound constraints

%the eval_bound doesn't work
fif(and(done(ah_askquestion(ID,all,ID1,_,_),_),
    and(done(copy_ireq_dreq(ID)),
    and(psreply(ID1,ID2),
    and(new_message(ID2,Asserts2),
    and(new_message_kv(ID2,Asserts3),
        eval_bound((mult_gather_all([ireq(_,ID),dreq(_,ID)],Asserts4),
	            append([Asserts2,Asserts3,Asserts4],Asserts5)),[Asserts2,Asserts3,ID])))))),
conclusion(call(ah_translate_queryans(ID,ID2,Asserts5),ID2))).

fif(error(ah_translate_queryans(ID,ID2,Asserts),ID2),
conclusion(call(senderrmsg(ID)))).

%fif(done(ah_translate_queryans(ID,ID2,Asserts1),ID3),
%conclusion(done(compute_dreq(ID)))).

fif(done(compute_dreq(ID)),
conclusion(compute_pact(ID))).

fif(and(compute_pact(ID),
    and(ireq(type(ID,'sa-request'),ID),
    and(ireq(sem(ID,PSem),ID),
    and(ireq(path(ID,Path),ID),
        ireq(to(Path,To),ID))))),
conclusion(pact(request_subplan,ID))).

%what about assoc_msgs?
fif(and(pact(request_subplan,ID),
    and(ireq(type(ID,'sa-request'),ID),
    and(ireq(sem(ID,PSem),ID),
    and(ireq(path(ID,Path),ID),
    and(ireq(to(Path,To),ID),
    and(or(and(done(ah_askquestion(ID,all,ID1,Asserts,Asserts1),ID),
           and(psreply(ID1,ID2),
           and(new_message(ID2,Asserts1a),
           and(new_message_kv(ID2,Asserts1b),
               eval_bound(append(Asserts1a,Asserts1b,Asserts1),[Asserts1a,Asserts1b]))))),
        eval_bound(Asserts1 = [],[])),
%these two lines caused alma to go on forever
    and(eval_bound(gather_all(ireq(_,ID),Asserts2),[ID]),
    and(eval_bound(gather_all(dreq(_,ID),Asserts3),[ID]),
        eval_bound(append([Asserts1,Asserts2,Asserts3],Asserts4),[Asserts1,Asserts2, Asserts3]))))))))),
conclusion(call(ah_reqnewsubplan(ID,all,ID3,Asserts4,Asserts5),ID2))).

fif(error(ah_reqnewsubplan(ID,all,ID4,Asserts1,Assert2),ID3),
conclusion(call(senderrmsg(ID)))).

%translate ps query reply
%shouldn't this be looking at ID2?
fif(and(done(ah_reqnewsubplan(ID,all,ID1,Asserts,Asserts1),ID3),
    and(psreply(ID1,ID2),
    and(new_message(ID2,Asserts2),
    and(new_message_kv(ID2,Asserts3),
    and(eval_bound(mult_gather_all([dreq(_,ID)],Asserts4),[ID]),
        eval_bound(append([Asserts1,Asserts2,Asserts3,Asserts4],Asserts5),[Asserts1,Asserts2,Asserts3])))))),
conclusion(call(ah_translate_plan(ID,ID2,Asserts5),ID2))).

fif(error(ah_translate_plan(ID,ID2,Asserts),ID2),
conclusion(call(senderrmsg(ID)))).

%if the plan checks out request that the ps update
fif(done(ah_translate_plan(ID,ID2,Asserts),ID2),
conclusion(done(compute_pact(ID)))).

fif(done(compute_pact(ID)),
conclusion(compute_eact(ID))).

%included ID1 in assoc_msgs?
fif(and(compute_eact(ID),
    and(done(ah_reqnewsubplan(ID,all,ID3,Asserts,Asserts1),ID2),
    and(psreply(ID3,ID4),
    and(new_message(ID4, Asserts2),
    and(new_message_kv(ID4, Asserts3),
    and(eval_bound(mult_gather_all([ireq(sem(ID,Sem),ID),ireq(lf(Sem,_),ID),dreq(_,ID),pact(_,ID)],Asserts4),[ID]),
        eval_bound(append([Asserts2,Asserts3,Asserts4,
                            [done(ah_reqnewsubplan(ID,all,ID3,dummy,dummy),ID2), psreply(ID3,ID4)]], 
                           Asserts5),[ID,Asserts2,Asserts3,Asserts4]))))))),
conclusion(call(ah_outmgrconfirm(ID,Asserts5),ID2))).

fif(and(compute_eact(ID),
    and(done(ah_translate_plan(ID,ID2,_),ID2),
    and(new_message(ID2,Asserts),
    and(new_message_kv(ID2,Asserts2),
    and(done(ah_reqnewsubplan(ID,all,ID1,_,_),ID3),
    and(psreply(ID1,ID2),
        eval_bound(append([Asserts,Asserts2,
                           [done(ah_reqnewsubplan(ID,all,ID1,_,_),ID3),psreply(ID1,ID2)]],
                          Asserts3),[ID,Asserts,Asserts2]))))))),
conclusion(call(ah_requpdateplan(ID,ID2,ID3,Asserts3),ID3))).

fif(and(compute_eact(ID),
    and(done(ah_requpdateplan(ID,ID2,ID3,Asserts),ID3),
    and(psreply(ID3,ID4),
    and(new_message(ID4,Asserts1),
    and(new_message_kv(ID4,Asserts2),
        eval_bound(append(Asserts1,Asserts2,Asserts3),[Asserts1,Asserts2])))))),
conclusion(call(ah_confirmsuccess(ID,ID4,Asserts3),ID4))).

fif(error(ah_requpdateplan(ID,ID2,ID3,Asserts),ID2),
conclusion(call(senderrmsg(ID)))).

fif(error(ah_confirmsuccess(ID,ID1,Asserts),ID1),
conclusion(call(senderrmsg(ID)))).

fif(done(ah_confirmsuccess(ID,ID1,Asserts),ID1),
conclusion(oact(plan_confirmed(ID,ID1),ID))).

fif(and(oact(plan_confirmed(ID,ID1),ID),
        done(ah_outmgrconfirm(ID,ID2,Asserts),ID2)),
conclusion(done(compute_eact))).

fif(and(reset_alma(ID),
        eval_bound(reset_alma,[])),
conclusion(and(alma_reset(ID),
               df(reset_alma(ID))))).








