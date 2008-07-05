// This is the header file just for the BufferOverFlowSecurityFlaw, other header files would be 
// defined, one for each security flaw. The attempt is to define a scalable system for handling 
// lots of security flaws.

// This file represents a single type of security flaw, with both locations in source code where 
// such flaws can be present, and ways in which such flaws can be seeded into source code:
//    1) The class BufferOverFlowSecurityFlaw
//       can have many nested classes to represent different ways in which the security flaw can
//       appear in source code (locations in the source code where it could exist).  The 
//       Vulnerability class is just one instance. 
//    2) The class BufferOverFlowSecurityFlaw can also have many nested classes to represent ways
//       in which the security flaw could be seeded into an application.  The SeedSecurityFlaw class
//       is just one instance.
// It may or may not be a one-to-one mapping between these nested classes.

class BufferOverFlowSecurityFlaw : public SecurityFlaw
   {
     public:

       // Note that there can be many vulnerabilities for a single security flaw (arrays indexing inside
       // a loop body, array indexing in a conditional test, indexing of a STL vector, etc.).
       // In general, vulnerabilities can be independent of the ways in which a security flaw can be seeded,
       // so this is no one-to-one mapping of Vulnerability classes to SeedSecurityFlaw classes.
          class Vulnerability
             {
               public:
                 // This class is only required where the specific security flaw's vulnerability detection requires it.
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined. In this case
                      // we detect buffer overflow (or under flow) vulnerabilities that appear in loops.  The 
                      // reason for this degree of specificity is that that such vulnerabilities can be seeded 
                      // in a wide range of ways (e.g. modifying the loop bounds, in addition to the modifications 
                      // to the array subscript expression).
                         public:
                              bool isLoop;
                              InheritedAttribute() : isLoop(false) {}

                           // Note that we don't really need a copy constructor, since the one generated by C++ 
                           // would work in this limited case, it is just a safe practice.
                              InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class Traversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );
                       };

            // This abstracts the details of setting up a call to the AST traversal.
               void detector( SgProject *project );
             };

       // Forward declaration (required for next two class declarations)
          class SeedSecurityFlaw;

       // Since we first find all the vulnerabilities and then seed them, this refinds the marked vulnerabilities 
       // after the first pass as part of the seeding process.  Since we have to find the vulnerability and then
       // backup within the AST subtree to a predefined level of grainularity, this traversal generates each AST 
       // copy and then applies the seeding to each copy.  As an alternative to marking the AST with AST persistant
       // attributes, we could have just saved a list of IR nodes where vulnerabilites were defined.  That might
       // make for a simple but more coupled implementation later!
          class CloneVulnerability
             {
            // Note that the cloned vulnerability might be the whole function as a rule, or some arbitrarily sized 
            // fragment of code that contains the security flaws vulnerability.  Clones that are constructed are
            // build with there marked security vulnerability attributes in place.  This allows for separate steps
            // to mark the different vulnerablities in each clone differently (to seed all or a subset of all
            // the vulnerabilities into the constructed clones).  At present the number of clones is equal to
            // the number of identified vulnerabilities, but this need not be the case in the future.  The goal
            // is to avoid seeding all vulnerabilities into a single clone since that would be unnecessarily 
            // messy (too many security flaws to be useful in the evaluation).  A concern is that a clone with 
            // too many security flaws would likely confuse a static analysis tool with no particular purpose.
            // The static analysis of massively flawed source code is likely not interesting.

            // This function is substantially independent of the type of security flaw being evaluated, so this 
            // class can likely go into the base class.
               public:
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined. In this case
                      // we detect buffer overflow (or under flow) vulnerabilities that appear in loops.  The 
                      // reason for this degree of specificity is that that such vulnerabilities can be seeded 
                      // in a wide range of ways (e.g. modifying the loop bounds, in addition to the modifications 
                      // to the array subscript expression).
                         public:
                              bool inOriginalCode;
                              InheritedAttribute() : inOriginalCode(false) {}

                           // Note that we don't really need a copy constructor, since the one generated by C++ 
                           // would work in this limited case, it is just a safe practice.
                              InheritedAttribute(const InheritedAttribute & X) : inOriginalCode(X.inOriginalCode) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class CloneVulnerabilityTraversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines what level of graniulatity to seed the security flaw (by building the clones).
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );

                           // Pointer to the BufferOverFlowSecurityFlaw (required in traversal visit() function)
                              SeedSecurityFlaw* associtedSeedSecurityFlaw;

                           // Constructor
                              CloneVulnerabilityTraversal(SeedSecurityFlaw* X);
                             ~CloneVulnerabilityTraversal();
                       };

                    class PrimaryVulnerabilityTraversal : public SgSimpleProcessing
                       {
                         public:
                              SgNode* primaryVulnerabilityNodeInClone;
                              SgNode* primaryVulnerabilityNodeInOriginalCode;
                              SgNode* rootOfClone;

                              void visit( SgNode* astNode );

                           // Constructor
                              PrimaryVulnerabilityTraversal(SgNode* primaryVulnerabilityNodeInClone, SgNode* primaryVulnerabilityNodeInOriginalCode, SgNode* rootOfClone )
                                 : primaryVulnerabilityNodeInClone(primaryVulnerabilityNodeInClone), 
                                   primaryVulnerabilityNodeInOriginalCode(primaryVulnerabilityNodeInOriginalCode), 
                                   rootOfClone(rootOfClone)
                                 {
                                // This function could not be defined in the source file (g++ 3.4.6 problem)
                                 }

                             ~PrimaryVulnerabilityTraversal()
                                 {
                                // This function could not be defined in the source file (g++ 3.4.6 problem)
                                 }
                       };

                 // Hide details of calling the traversal
                    static void makeClones( SgProject* project, SeedSecurityFlaw* flaw );

                 // If primaryNodeInClonedCode is always NULL then we should remove it from the argument parameter list.
                    static void markPrimaryCloneVulnerability( SgNode* primaryNodeInClonedCode, SgNode* primaryNodeInOriginalCode, SgNode* rootOfClone );
             };


          class MarkClones
             {
            // Note that the cloned vulnerability have all vulnerabilities marked (copied from the original code).
            // This step marks the IR nodes contianing vulnerabilities with additional attributes that control
            // which vulnerabilities will be seeded in a separate step.  This function is substantially independent
            // of the type of security flaw being evaluated, so this class can likely go into the base class.

               public:
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined. In this case
                      // we detect buffer overflow (or under flow) vulnerabilities that appear in loops.  The 
                      // reason for this degree of specificity is that that such vulnerabilities can be seeded 
                      // in a wide range of ways (e.g. modifying the loop bounds, in addition to the modifications 
                      // to the array subscript expression).
                         public:
                              bool inOriginalCode;
                              bool inClonedCode;

                              InheritedAttribute() : inOriginalCode(false), inClonedCode(false) {}

                           // Note that we don't really need a copy constructor, since the one generated by C++ 
                           // would work in this limited case, it is just a safe practice.
                              InheritedAttribute(const InheritedAttribute & X) : inOriginalCode(X.inOriginalCode), inClonedCode(X.inClonedCode) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class MarkClonesTraversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines what level of graniulatity to seed the security flaw (by building the clones).
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );

                           // Pointer to the BufferOverFlowSecurityFlaw (required in traversal visit() function)
                              SeedSecurityFlaw* associtedSeedSecurityFlaw;

                           // Constructor
                              MarkClonesTraversal(SeedSecurityFlaw* X);
                             ~MarkClonesTraversal();
                       };

                // Hide details of calling the traversal
                   static void markVulnerabilitiesInClones( SgProject* project, SeedSecurityFlaw* flaw );
             };

       // Note that there can be many ways to seed a security flaw into an application 
       // (or generate it separately from it being seeded into an existing application).
          class SeedSecurityFlaw
             {
            // This class introduces a single kind of seeding at either a specific grainularity 
            // (file, function, block, statement) or using a specific mechanism to hide it as a 
            // security flaw (hidden behind modification of array indexing, or behind a modification 
            // to the loop bound, etc.).

               public:
                 // This value is used to control to AST clone generation to support seeding, but only 
                 // when (seedOriginalCode == false).
                    GrainularitySpecification seedGrainulatity;

                    bool seedOriginalCode;

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class SeedTraversal : public SgSimpleProcessing
                       {
                         public:
                           // This function defines how to seed the security flaw at
                           // the previously detected location of the vulnerability
                              void visit( SgNode* node );

                           // std::vector<SgNode*> grainularityOfSeededCode( SgNode* astNode );
                       };

            // Constructor and virtual destructor
               SeedSecurityFlaw();
               virtual ~SeedSecurityFlaw();

               bool get_seedOriginalCode();
               void set_seedOriginalCode( bool t );

            // For any security flaw marked previously as a vulnerability, back-track up the AST to a subtree 
            // to copy so that the seeded security flaw can be introduced in the copy (so that we can leave 
            // in place the original code associated with the security vulnerability.
            // New function to generate a vector of positions at which to build subtrees.
            // static std::vector<SgNode*> grainularityOfSeededCode( SgNode* astNode );
               std::vector<SgNode*> grainularityOfSeededCode( SgNode* astNode );

               void seed( SgNode *astNode );
             };

       // Collection of different sorts of vulnerabilities that we want to identify as a buffer over flow 
       // security flaw (there can be many).  This data member might be placed into the base class and
       // a base class provided for the Vulnerability class.
          std::vector<Vulnerability*> vulnerabilityKindList;

       // Collection of different way to seed buffer overflow security flaws into code (in a loop, in a 
       // conditional test, etc.).  This data member might be placed into the base class and
       // a base class provided for the SeedSecurityFlaw class.
          std::vector<SeedSecurityFlaw*> seedKindList;

       // Constructor
          BufferOverFlowSecurityFlaw();

       // Virtual destructor (since we have virtual functions in this class)
          virtual ~BufferOverFlowSecurityFlaw();

       // Marks all locations in source code where this type of security flaw could exist (marks with AST 
       // attribute). This function uses the vulnerabilityKindList to address each kind of source code 
       // vulnerability for this specific type of security flaw.
          void detectVunerabilities( SgProject *project );

       // This relocates the positions of marked vulnerabilities so that we can trace backup the AST to a specific 
       // level of grainularity, make a copy of the subtree, and then seed the subtree instead of the original code.
       // This way all security flaws are not forced into a single instatiation of the code.
       // void seedWithGrainularity( SgProject *project );
          void codeCloneGeneration( SgProject *project );

       // Seeds this security flaw into the marked location of a potential vulnerability (using multiple 
       // ways of seeding the flaw as defined in the seedKindList).
          void seedSecurityFlaws( SgProject *project );
   };

