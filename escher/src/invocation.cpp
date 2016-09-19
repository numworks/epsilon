#include <escher/invocation.h>

Invocation::Invocation(Action a, void * c) :
	m_action(a),
	m_context(c)
{
}

void Invocation::perform(void * sender) {
	(*m_action)(m_context, sender);
}
