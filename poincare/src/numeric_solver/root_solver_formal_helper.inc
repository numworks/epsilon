

// ------- RootSolverFormalHelper implementation -------

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextDetailedRoot() {
  using namespace Internal;
  const Tree* e = getTree();
  if (e == nullptr || e->hasDescendantSatisfying(
                          [](const Tree* t) { return t->isRandomized(); })) {
    return k_nanRoot;
  }

  switch (e->type()) {
    case Type::Mult:
      /* x*y = 0 => x = 0 or y = 0 */
      return registerDetailedRoot(nextRootInMultiplication());

    case Type::Add:
    case Type::Sub:
      return registerDetailedRoot(nextRootInAddition());

    case Type::Pow:
    case Type::PowReal:
    case Type::Root:
    case Type::Div:
      /* f(x,y) = 0 => x = 0 */
      return registerDetailedRoot(nextRootSearchingInChild(0));

    case Type::Abs:

    case Type::ATan:
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
    case Type::SinH:
#endif
    case Type::Opposite:
    case Type::Sqrt: {
      /* f(x) = 0 <=> x = 0 */
      TSolver childSolver(as().m_stepper.start(), as().m_stepper.end(),
                          e->child(0));
      return registerDetailedRoot(childSolver.nextDetailedRoot());
    }

    case Type::Dep:
      return registerDetailedRoot(nextRootInDependency());

    default:
      if (!GetComplexProperties(e).canBeNull()) {
        return k_nanRoot;
      }
      // Use classic numeric scan
      Solution<double> res = as().parentNext();
      if (res.interest != Interest::None) {
        res.interest = Interest::Root;
      }
      return {.x = res.x, .nullDescendant = e};
  }
}

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextRootSearchingInChild(
    int childIndex) const {
  using namespace Internal;
  const Tree* e = getTree();
  const Tree* child = e->child(childIndex);

  TSolver childSolver(as().m_stepper.start(), as().m_stepper.end(), child);

  DetailedRoot root;
  while (std::isfinite(
      (root = childSolver.nextDetailedRoot()).x)) {  // assignment in condition
    /* Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x = -1.
     *
     * Sometimes xRoot is not precise enough and e can approximate to undef
     * even if it's not. e.g. f(x)=sqrt(cos(x)), when searching roots of
     * cos(x), sometimes cos(xRoot) = -0.0..01, so sqrt(cos(xRoot)) = undef.
     * Avoid this by cloning e and replacing cos(xRoot) by 0. */
    Tree* ebis = e->cloneTree();
    ebis->child(childIndex)->cloneTreeOverTree(0_de);
    double value = Internal::Approximation::To<double>(
        ebis, root.x,
        Internal::Approximation::Parameters{
            .policy = {.optimize = true},
            .expressionParameters = Internal::Approximation::Prepared{}});
    ebis->removeTree();
    if (std::fabs(value) < TSolver::NumericHelper::NullTolerance(root.x)) {
      return {.x = root.x, .nullDescendant = root.nullDescendant};
    }
  }
  return k_nanRoot;
}

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextRootSearchingInChildren(
    ExpressionTestAuxiliary test) const {
  using namespace Internal;
  const Tree* e = getTree();
  DetailedRoot root = k_nanRoot;
  int n = e->numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (test(e->child(i))) {
      DetailedRoot rootChild = nextRootSearchingInChild(i);
      if (std::isfinite(rootChild.x) &&
          (!std::isfinite(root.x) ||
           std::fabs(as().m_stepper.start() - rootChild.x) <
               std::fabs(as().m_stepper.start() - root.x))) {
        root = rootChild;
      }
    }
  }
  return root;
}

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextRootInMultiplication() const {
  using namespace Internal;
  assert(getTree()->isMult());
  return nextRootSearchingInChildren([](const Tree*) { return true; });
}

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextRootInAddition() const {
  using namespace Internal;
  /* Special case for expressions of the form "f(x)^a+g(x)", with:
   * - f(x) and g(x) sharing a root x0
   * - f(x) being defined only on one side of x0
   * - 0 < a
   * Since the expression does not change sign around x0, the usual numerical
   * schemes won't work. We instead look for the zeroes of f, and check whether
   * they are zeroes of the whole expression. */
  ExpressionTestAuxiliary test = [](const Tree* e) {
    return e->hasDescendantSatisfying([](const Tree* e) {
      double exponent = static_cast<double>(NAN);
      if (e->type() == Type::Sqrt) {
        exponent = 0.5;
      } else if (e->type() == Type::Pow || e->type() == Type::PowReal) {
        exponent = Internal::Approximation::To<double>(
            e->child(1),
            Internal::Approximation::Parameters{.policy = {.optimize = true}});
      } else if (e->type() == Type::Root) {
        exponent = 1. / Internal::Approximation::To<double>(
                            e->child(1), Internal::Approximation::Parameters{
                                             .policy = {.optimize = true}});
      }
      if (std::isnan(exponent)) {
        return false;
      }
      return static_cast<double>(0) < exponent;
    });
  };
  DetailedRoot childrenRoot = nextRootSearchingInChildren(test);

  // Compare result with classic numeric scan
  TSolver solver = as();
  DetailedRoot root = {.x = solver.parentNext().x, .nullDescendant = getTree()};
  if (!std::isfinite(root.x) ||
      std::fabs(childrenRoot.x - as().m_stepper.start()) <
          std::fabs(root.x - as().m_stepper.start())) {
    root = childrenRoot;
  }
  return root;
}

template <typename TSolver>
typename RootSolverFormalHelper<TSolver>::DetailedRoot
RootSolverFormalHelper<TSolver>::nextRootInDependency() const {
  using namespace Internal;
  const Tree* e = getTree();
  assert(e->isDep());
  const Tree* main = Dependency::Main(e);
  const Tree* depsList = Dependency::Dependencies(e);
  TSolver depSolver(as().m_stepper.start(), as().m_stepper.end(), main);

  DetailedRoot root;
  // Find root in main and then check that the dependencies are not undefined
  while (std::isfinite((root = depSolver.nextDetailedRoot()).x)) {
    /* Replace nullDescendant by 0 in deps list. This ensures that
     * approximation errors do not invalidate the dependencies check.
     *
     * For example, when solving `sqrt(0.5x^2-1)=0`, one found root is
     * slightly below `sqrt(2)`, making the dependency undefined. By replacing
     * `0.5x^2-1` by `0`, this dependency check doesn't fail. */
    Tree* depsListClone = depsList->cloneTree();
    depsListClone->deepReplaceWith(root.nullDescendant, 0_e);
    bool definedDeps = true;
    for (const Tree* childDep : depsListClone->children()) {
      double value = Internal::Approximation::To<double>(
          childDep, root.x,
          Internal::Approximation::Parameters{.policy = {.optimize = true}});
      if (std::isnan(value)) {
        definedDeps = false;
        break;
      }
    }
    depsListClone->removeTree();
    if (definedDeps) {
      break;
    }
  }

  return {.x = root.x, .nullDescendant = root.nullDescendant};
}
